#include "vdf.hpp"
#include <set>
#include <source-parsers-shared/internal/peekable-stream.hpp>
#include "errors.hpp"

namespace VdfParser {
  using namespace SourceParsers::Internal;

  namespace {
    const std::set WHITESPACE = { ' ', '\t', '\r', '\n' };

    std::string parseLiteral(PeekableStream& stream) {
      std::set<char> terminators;
      auto isQuoted = false;

      if (stream.peek() == '"') {
        stream.discard();
        terminators = { '"' };
        isQuoted = true;
      } else {
        terminators = { '"', '{', '}' };
        terminators.insert(WHITESPACE.begin(), WHITESPACE.end());
        isQuoted = false;
      }

      std::string literal = stream.consumeWhile(
        [&terminators](const char c) {
          return !terminators.contains(c);
        }
      );

      if (isQuoted) {
        stream.discard();
      }

      return std::move(literal);
    }

    void discardWhitespaceAndComments(PeekableStream& stream) {
      while (!stream.empty() && (WHITESPACE.contains(stream.peek()) || stream.peek(2) == "//")) {
        if (WHITESPACE.contains(stream.peek())) {
          stream.discardWhile(
            [](const char c) {
              return WHITESPACE.contains(c);
            }
          );
        }

        if (stream.peek(2) == "//") {
          stream.discardWhile(
            [](const char c) {
              return c != '\r' && c != '\n';
            }
          );
        }
      }
    }

    CaseInsensitiveMap<KeyValue> parseKeyValues(PeekableStream& stream) {
      CaseInsensitiveMap<KeyValue> values;

      while (!stream.empty() && stream.peek() != '}') {
        KeyValue keyValue;

        discardWhitespaceAndComments(stream);

        auto key = parseLiteral(stream);

        discardWhitespaceAndComments(stream);

        if (stream.peek() == '{') {
          stream.discard();
          keyValue.value = parseKeyValues(stream);

          if (stream.peek() != '}') {
            throw Errors::UnexpectedCharacter("Expected '}' to close key-value block");
          }

          stream.discard();
        } else {
          keyValue.value = parseLiteral(stream);
        }

        values.emplace(std::move(key), std::move(keyValue));

        discardWhitespaceAndComments(stream);
      }

      return std::move(values);
    }
  }

  KeyValue fromString(const std::string& raw) {
    PeekableStream stream(raw);
    return KeyValue{ .value = parseKeyValues(stream) };
  }
}
