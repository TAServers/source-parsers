#pragma once

#include <array>
#include <cstdint>
#include "common.hpp"
#include "../enums.hpp"
#include "../limits.hpp"

namespace MdlParser::Structs::Mdl {
  using Limits::MAX_NUM_LODS;

#pragma pack(push, 1)

  // These are not used and are only here for alignment
  // Pointers have been replaced with int32 to mimic a 32-bit system
  struct mstudio_modelvertexdata_t {
    int32_t pVertexData;
    int32_t pTangentData;
  };

  struct mstudio_meshvertexdata_t {
    int32_t modelvertexdata;
    std::array<int32_t, MAX_NUM_LODS> numLODVertexes;
  };

  struct Bone {
    int32_t szNameIndex;

    int32_t parent;
    std::array<int32_t, 6> boneControllers;

    Vector pos;
    Quaternion quat;
    RadianEuler rot;

    Vector posScale;
    Vector rotScale;

    Matrix3x4 poseToBone;
    Quaternion qAlignment;

    int32_t flags;

    int32_t procType;
    int32_t procIndex;

    int32_t physicsBoneIndex;
    int32_t surfacePropIndex;

    int32_t contents;

    std::array<int32_t, 8> unused;
  };

  struct Texture {
    int32_t szNameIndex;

    // TODO: Determine matching enum for these
    int32_t flags;

    int32_t used; // ?????
    int32_t unused1;

    // Two pointers that are unused even in engine
    int32_t material;
    int32_t clientmaterial;

    std::array<int32_t, 10> unused;
  };

  struct Flex {
    int32_t flexDesc;

    float target0;
    float target1;
    float target2;
    float target3;

    int32_t vertsCount;
    int32_t vertsOffset;

    int32_t flexPair;
    Enums::Mdl::VertAnimType vertAnimType;

    std::array<uint8_t, 3> unused0;
    std::array<int32_t, 6> unused1;
  };

  struct Mesh {
    int32_t material;

    int32_t modelIndex;

    int32_t vertsCount;
    int32_t vertsOffset;

    int32_t flexesCount;
    int32_t flexesOffset;

    int32_t materialType;
    int32_t materialParam;

    int32_t meshId;

    Vector centre;

    // Unused, only for alignment
    mstudio_meshvertexdata_t vertexdata;

    std::array<int32_t, 8> unused;
  };

  struct Model {
    std::array<char, 64> name;

    int32_t type;

    float boundingRadius;

    int32_t meshesCount;
    int32_t meshesOffset;

    int32_t vertsCount;
    int32_t vertsOffset;
    int32_t tangentsOffset;

    int32_t attachmentsCount;
    int32_t attachmentsOffset;

    int32_t eyeballsCount;
    int32_t eyeballsOffset;

    // Unused, only for alignment
    mstudio_modelvertexdata_t vertexdata;

    std::array<int32_t, 8> unused;
  };

  struct BodyPart {
    int32_t szNameIndex;

    int32_t modelsCount;
    int32_t base;
    int32_t modelsOffset;
  };

  struct Header {
    static const int32_t MAX_SUPPORTED_VERSION = 48;

    int32_t id; // Model format ID (IDST)
    int32_t version;
    int32_t checksum;
    std::array<char, 64> name;
    int32_t dataLength; // Size of the file

    Vector eyePosition;
    Vector illumPosition;

    Vector hullMin;
    Vector hullMax;
    Vector viewMin; // Valve dev wiki doesn't know what this is, but likely the render bounds
    Vector viewMax;

    Enums::Mdl::Flags flags;

    int32_t boneCount;
    int32_t boneOffset;

    int32_t boneControllerCount;
    int32_t boneControllerOffset;

    int32_t hitboxCount;
    int32_t hitboxOffset;

    int32_t localAnimCount;
    int32_t localAnimOffset;

    int32_t localSequenceCount;
    int32_t localSequenceOffset;

    int32_t activityListVersion;
    int32_t eventsIndexed;

    int32_t textureCount;
    int32_t textureOffset;

    int32_t textureDirCount;
    int32_t textureDirOffset;

    int32_t skinRefCount;
    int32_t skinFamilyCount;
    int32_t skinRefOffset;

    int32_t bodypartCount;
    int32_t bodypartOffset;

    int32_t attachmentCount;
    int32_t attachmentOffset;

    int32_t localnodeCount;
    int32_t localnodeOffset;
    int32_t localnodeNameOffset;

    int32_t flexDescCount;
    int32_t flexDescOffset;

    int32_t flexControllerCount;
    int32_t flexControllerOffset;

    int32_t flexRulesCount;
    int32_t flexRulesOffset;

    int32_t ikChainCount;
    int32_t ikChainOffset;

    int32_t mouthsCount;
    int32_t mouthsOffset;

    int32_t localPoseParamCount;
    int32_t localPoseParamIndex;

    int32_t surfacePropIndex;

    int32_t keyvalueOffset;
    int32_t keyvalueCount;

    int32_t ikLockCount;
    int32_t ikLockIndex;

    float mass;
    int32_t
    contents;
    // Valve dev wiki doesn't know what this is but most likely contents flags https://wiki.facepunch.com/gmod/Enums/CONTENTS

    int32_t includeModelCount;
    int32_t includeModelOffset;

    int32_t virtualModel;

    int32_t animBlocksNameOffset;
    int32_t animBlocksCount;
    int32_t animBlocksOffset;

    int32_t animBlockModel;

    int32_t boneTableNameOffset;

    int32_t vertexBase;
    int32_t offsetBase;

    uint8_t directionalDotProduct;

    uint8_t rootLoD;

    uint8_t numAllowedRootLoDs;

    uint8_t unused0;
    int32_t unused1;

    int32_t flexControllerUICount;
    int32_t flexControllerUIOffset;

    float vertAnimFixedPointScale;
    int32_t unused2;

    int32_t header2Offset;
    int32_t unused3;
  };

  struct Header2 {
    int32_t srcBoneTransformCount;
    int32_t srcBoneTransformOffset;

    int32_t illumPositionAttachmentOffset;

    float maxEyeDeflection;

    int32_t linearBoneOffset;

    int32_t szNameIndex;

    int32_t boneFlexDriverCount;
    int32_t boneFlexDriverOffset;

    std::array<int32_t, 56> reserved;
  };

#pragma pack(pop)
}
