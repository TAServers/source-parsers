#include "vpk.hpp"
#include <ranges>
#include <set>
#include <source-parsers-shared/errors.hpp>
#include <source-parsers-shared/internal/case-insensitive-map.hpp>
#include <source-parsers-shared/internal/offset-data-view.hpp>
#include "structs/directory-entry.hpp"
#include "structs/headers.hpp"

namespace VpkParser {
  using namespace SourceParsers::Errors;
  using namespace SourceParsers::Internal;
  using namespace Structs;

  namespace {
    constexpr uint32_t FILE_SIGNATURE = 0x55aa1234;
    const std::set<uint32_t> SUPPORTED_VERSIONS = { 1, 2 };
  }

  Vpk::Vpk(const std::span<std::byte>& data) {
    const OffsetDataView dataView(data);
    const auto header = dataView.parseStruct<HeaderV1>(0, "Failed to parse base VPK header").first;

    if (header.signature != FILE_SIGNATURE) {
      throw InvalidHeader("VPK signature does not equal 0x55aa1234");
    }

    if (!SUPPORTED_VERSIONS.contains(header.version)) {
      throw UnsupportedVersion("VPK version not supported (supported versions are 1 and 2)");
    }

    size_t offset = header.version == 1 ? sizeof(HeaderV1) : sizeof(HeaderV2);
    while (true) {
      auto extension = dataView.parseString(offset, "Failed to parse extension");
      offset += extension.length() + 1;
      if (extension.empty()) {
        break;
      }
      extension = "." + extension;

      files.emplace(extension, CaseInsensitiveMap<CaseInsensitiveMap<File>>());

      while (true) {
        auto directory = dataView.parseString(offset, "Failed to parse directory");
        offset += directory.length() + 1;
        if (directory.empty()) {
          break;
        }

        // ASSUMPTION: Top-level files are encoded using a directory of a single space
        // Format can't use an empty string for this, as that terminates the section
        // Could use `/`, but that would be inconsistent with the other directory formats
        if (directory == " ") {
          directory = "";
        }

        files.at(extension).emplace(directory, CaseInsensitiveMap<File>());

        while (true) {
          const auto filename = dataView.parseString(offset, "Failed to parse filename");
          offset += filename.length() + 1;
          if (filename.empty()) {
            break;
          }

          const auto directoryInfo =
            dataView.parseStruct<DirectoryEntry>(offset, "Failed to parse directory entry").first;
          offset += sizeof(DirectoryEntry);

          files.at(extension).at(directory).emplace(
            filename,
            File{
              .archiveIndex = directoryInfo.archiveIndex,
              .offset = directoryInfo.entryOffset,
              .size = directoryInfo.entrySize,
              .preloadData = dataView.parseStructArrayWithoutOffsets<std::byte>(
                offset,
                directoryInfo.preloadDataSize,
                "Failed to parse preload data"
              ),
            }
          );

          offset += directoryInfo.preloadDataSize;
        }
      }
    }
  }

  const std::vector<std::byte>& Vpk::getPreloadData(const std::filesystem::path& path) const {
    return getFileMetadata(path).preloadData;
  }

  std::vector<std::byte> Vpk::readFile(
    const std::filesystem::path& path,
    const std::function<std::vector<std::byte>(uint16_t archive, uint32_t offset, uint32_t size)>& readFromArchive
  ) const {
    const auto& fileInfo = getFileMetadata(path);
    const auto& archiveData = readFromArchive(fileInfo.archiveIndex, fileInfo.offset, fileInfo.size);

    std::vector<std::byte> fileData;
    fileData.reserve(fileInfo.preloadData.size() + fileInfo.size);

    fileData.insert(fileData.begin(), fileInfo.preloadData.begin(), fileInfo.preloadData.end());
    fileData.insert(fileData.end(), archiveData.begin(), archiveData.end());

    return std::move(fileData);
  }

  DirectoryContents Vpk::list(const std::filesystem::path& path) const {
    const auto normalisedPath = getVpkDirectory(path);

    std::set<std::filesystem::path> fileList = {};
    std::set<std::filesystem::path> directoryList = {};

    for (const auto& [extension, directories] : files) {
      for (const auto& [directory, fileNames] : directories) {
        auto subdirectory = getSubdirectory(normalisedPath, directory);

        if (subdirectory.has_value()) {
          directoryList.emplace(std::move(subdirectory.value()));
        } else if (directory == normalisedPath) {
          for (const auto& fileName : fileNames | std::views::keys) {
            fileList.emplace(fileName + extension);
          }
        }
      }
    }

    std::erase_if(
      directoryList,
      [](const auto& dir) {
        return dir == "";
      }
    );
    return DirectoryContents{
      .directories = std::move(directoryList),
      .files = std::move(fileList),
    };
  }

  bool Vpk::fileExists(const std::filesystem::path& path) const {
    const auto components = splitPath(path);

    return files.contains(components.extension) //
      && files.at(components.extension).contains(components.directory) //
      && files.at(components.extension).at(components.directory).contains(components.filename);
  }

  const Vpk::File& Vpk::getFileMetadata(const std::filesystem::path& path) const {
    const auto components = splitPath(path);

    return files.at(components.extension).at(components.directory).at(components.filename);
  }

  Vpk::PathComponents Vpk::splitPath(const std::filesystem::path& path) {
    return {
      .extension = path.extension().generic_string(),
      .directory = getVpkDirectory(path.parent_path()),
      .filename = path.stem().generic_string(),
    };
  }

  std::string Vpk::getVpkDirectory(const std::filesystem::path& path) {
    auto formatted = path.generic_string();

    if (formatted.starts_with('/')) {
      if (formatted.length() > 1) {
        formatted.erase(0, 1);
      } else {
        formatted = "";
      }
    }

    if (!formatted.empty() && formatted.back() == '/') {
      formatted.pop_back();
    }

    return std::move(formatted);
  }

  std::optional<std::string> Vpk::getSubdirectory(
    const std::string& parentDirectory,
    const std::string& childDirectory
  ) {
    if (childDirectory.empty()) {
      return std::nullopt;
    }

    if (parentDirectory.empty()) {
      return childDirectory.substr(0, childDirectory.find_first_of('/'));
    }

    // Plus 1 for expected '/' between the matched child directory and next folder
    // Plus another 1 for the expected folder name after the '/'
    if (childDirectory.length() < parentDirectory.length() + 2 //
      || !childDirectory.starts_with(parentDirectory) //
      || childDirectory[parentDirectory.length()] != '/') {
      return std::nullopt;
    }

    const auto startIndex = parentDirectory.length() + 1; // Plus 1 to skip over slash
    const auto endIndex = childDirectory.find_first_of('/', startIndex);

    if (endIndex == std::string::npos) {
      return childDirectory.substr(startIndex);
    }

    return childDirectory.substr(startIndex, endIndex - startIndex);
  }
}
