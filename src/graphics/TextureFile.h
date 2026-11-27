#pragma once
#include "engine/Binaryfile.h"

struct BinaryFile("ATEX", 0) Texturefile : Engine::BinaryFileBaseHeader {
    enum class TextureFormat {
        RGBA8888 = 0,
        DXT1 = 1,
        DXT3 = 2,
        DXT5 = 3,
        BC4 = 4,
        BC5 = 5,
        RGBA8888_SRGB = 6,
        DXT1_SRGB = 7,
        DXT3_SRGB = 8,
        DXT5_SRGB = 9
    };
    enum class TextureFlags : uint32_t {
        // Results in mirrored repeating.
        Mirror_U = 1,
        Mirror_V = 2,
        // Clamps the texture to border, takes priority over mirror in case of both being defined for an axis.
        Clamp_U = 4,
        Clamp_V = 8
    };

    struct MipLump_t {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t DataSize = 0;
    };

    Engine::BinaryFileLump<MipLump_t> MipLump;
    Engine::BinaryFileLump<char> DataLump;
    uint32_t Flags = 0;
    uint16_t Format = 0;
};