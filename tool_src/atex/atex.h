#pragma once
#include "graphics/TextureFile.h"

using TextureFormat = Texturefile::TextureFormat;
using MipLump_t = Texturefile::MipLump_t;
using TextureFlags = Texturefile::TextureFlags;

// Note: This is as it is passed to the compression algorithms, so the things like DXT1 are set as 4 channels.
inline int GetAmountOfChannelsFromTextureFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::RGBA8888:
    case TextureFormat::RGBA8888_SRGB:
    case TextureFormat::DXT1:
    case TextureFormat::DXT3:
    case TextureFormat::DXT5:
    case TextureFormat::DXT1_SRGB:
    case TextureFormat::DXT3_SRGB:
    case TextureFormat::DXT5_SRGB:
        return 4;
    case TextureFormat::BC4:
        return 1;
    case TextureFormat::BC5:
        return 2;
    }

    Engine::Error("what");
}