#include "STDGLTexture.h"

using TextureFormat = Texturefile::TextureFormat;
using TextureFlags = Texturefile::TextureFlags;

GLuint STDGLTextureSystem::MakeTextureObject(std::span<char> buffer, const std::string& pathforerrormessages) {
    auto& File = *std::get<Texturefile*>(Engine::BinaryFileImporter<Texturefile>::Import(std::as_writable_bytes(std::span(buffer)), pathforerrormessages));

    GLuint tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    SetFilteringOnTexture(tex);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
    auto& Mips = File.MipLump;
    auto& Data = File.DataLump;

    GLuint textureformat;
    GLuint subimageformat;
    bool isCompressed = true;
    switch (TextureFormat(File.Format)) {
    case TextureFormat::RGBA8888:
        textureformat = GL_RGBA8;
        subimageformat = GL_RGBA;
        isCompressed = false;
    break;
    case TextureFormat::RGBA8888_SRGB:
        textureformat = GL_SRGB8_ALPHA8;
        subimageformat = GL_RGBA;
        isCompressed = false;
    break;
    case TextureFormat::DXT1:
        textureformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    break;
    case TextureFormat::DXT1_SRGB:
        textureformat = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
    break;
    case TextureFormat::DXT3:
        textureformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    break;
    case TextureFormat::DXT3_SRGB:
        textureformat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
    break;
    case TextureFormat::DXT5:
        textureformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    break;
    case TextureFormat::BC4:
        textureformat = GL_COMPRESSED_RED_RGTC1;
    break;
    case TextureFormat::BC5:
        textureformat = GL_COMPRESSED_RG_RGTC2;
    break;
    case TextureFormat::DXT5_SRGB:
        textureformat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
    break;
    default:
        Engine::Error("Unknown texture format in file: " + pathforerrormessages);
    }

    if (File.Flags & uint32_t(TextureFlags::Mirror_U)) glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    if (File.Flags & uint32_t(TextureFlags::Mirror_V)) glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    if (File.Flags & uint32_t(TextureFlags::Clamp_U)) glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    if (File.Flags & uint32_t(TextureFlags::Clamp_V)) glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(tex, Mips.size(), textureformat, Mips[0].Width, Mips[0].Height);
    uint32_t Offset = 0;
    for (int MipLevel = 0; auto& Mip : Mips) {
        if (!isCompressed) {
            glTextureSubImage2D(tex, MipLevel, 0, 0, Mip.Width, Mip.Height, subimageformat, GL_UNSIGNED_BYTE, Data.begin() + Offset);
        } else {
            glCompressedTextureSubImage2D(tex, MipLevel, 0, 0, Mip.Width, Mip.Height, textureformat, Mip.DataSize, Data.begin() + Offset);
        }
        Offset += Mip.DataSize;
        MipLevel++;
    }

    return tex;
}

Engine::Reference<STDGLTexture> STDGLTextureSystem::GetTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }

    auto path = "materials/" + name;
    auto Texturefilestream = Engine::GetFileAsStream(path, std::ios::binary);
    if (!Texturefilestream) {
        return GetTexture("error.atex");
    }

    std::vector<char> buffer;
    Texturefilestream.seekg(0, std::ios::end);
    buffer.resize(Texturefilestream.tellg());
    Texturefilestream.seekg(0, std::ios::beg);
    buffer.assign(std::istreambuf_iterator<char>{Texturefilestream}, {});

    auto ret = new Engine::ManagedResource<STDGLTextureSystem, STDGLTexture>(this, MakeTextureObject(buffer, path));
    ret->resource.iteratorintexturemap = textures.emplace(name, ret).first;
    return ret;
}

void STDGLTextureSystem::BindfulOnlySetFiltering(FilterSettings newfilter) {
    filtering = newfilter;

    for (const auto& tex : textures) {
        SetFilteringOnTexture(tex.second->resource.GetTexture());
    }
}

void STDGLTextureSystem::SetFilteringOnTexture(GLuint tex) {
    // Account for no-mip textures.
    int mipcount;
    glGetTextureParameteriv(tex, GL_TEXTURE_IMMUTABLE_LEVELS, &mipcount);
    if (mipcount == 1) {
        switch(filtering) {
        case FilterSettings::Point:
            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        return;
        default:
            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return;
        }
    }

    // Mipped textures.
    switch(filtering) {
    case FilterSettings::Point:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return;
    case FilterSettings::Bilinear:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return;
    case FilterSettings::Trilinear:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return;
    case FilterSettings::Anisotropic_2X:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameterf(tex, GL_TEXTURE_MAX_ANISOTROPY, 2.0f);
    return;
    case FilterSettings::Anisotropic_4X:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameterf(tex, GL_TEXTURE_MAX_ANISOTROPY, 4.0f);
    return;
    case FilterSettings::Anisotropic_8X:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameterf(tex, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
    return;
    case FilterSettings::Anisotropic_16X:
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameterf(tex, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
    return;
    }
}