#include <bit>
#include "atex.h"
#include "texturecompression.h"
#include "graphics/TextureFile.h"
#include "engine/Filesystem.h"
#include "engine/ADF.h"
#include "stb_image.h"
#include "stb_image_resize2.h"

struct ADFSerialize ImportConfigurationFile {
    std::string Texture;
    std::string TextureFormat;
    bool NoMipMaps;
    bool Mirror_U = false;
    bool Mirror_V = false;
    bool Clamp_U = false;
    bool Clamp_V = false;
};

// Parameters for tool.
std::filesystem::path inpath = "";
std::filesystem::path outpath = "";
ImportConfigurationFile config;

// Storage for all the texture data.
std::vector<MipLump_t> MipLump;
std::vector<char> DataLump;
uint32_t Flags;

std::vector<unsigned char> StripTextureOfExtraData(std::vector<unsigned char> originaldata, TextureFormat format) {
    int channelcount = GetAmountOfChannelsFromTextureFormat(format);

    std::vector<unsigned char> ret;
    ret.reserve(originaldata.size() / 4 * channelcount);

    for (int pixel = 0; pixel < originaldata.size(); pixel += 4) {
        ret.insert(ret.end(), originaldata.begin() + pixel, originaldata.begin() + pixel + channelcount);
    }

    return ret;
}

int main(int argc, char* argv[]) {
    // Process arguments.
    std::vector<char*> args = {argv, argv + argc};
    args.erase(args.begin()); // Because working directory.

    while (!args.empty()) { // TODO: Add help string.
        std::string_view arg = args[0];

        if (arg == "-file") {
            args.erase(args.begin());
            if (args.empty()) {
                Engine::Error("Parameter for -file is missing");
            }

            inpath = std::string(args[0]);
            args.erase(args.begin());
        }

        if (arg == "-out") {
            args.erase(args.begin());
            if (args.empty()) {
                Engine::Error("Parameter for -out is missing");
            }

            outpath = std::string(args[0]) + "/" + inpath.filename().replace_extension("atex").string();
            args.erase(args.begin());
        }

    }

    ADFEntry::FromFile(inpath.string())["Texture"].Deserialize(config);

    // Parse configuration file.
    TextureFormat textureformat;
    if (config.TextureFormat == "RGBA8888") {
        textureformat = TextureFormat::RGBA8888;
    } else if (config.TextureFormat == "DXT1") {
        textureformat = TextureFormat::DXT1;
    } else if (config.TextureFormat == "DXT5") {
        textureformat = TextureFormat::DXT5;
    } else if (config.TextureFormat == "DXT3") {
        textureformat = TextureFormat::DXT3;
    } else if (config.TextureFormat == "RGBA8888_SRGB") {
        textureformat = TextureFormat::RGBA8888_SRGB;
    } else if (config.TextureFormat == "DXT1_SRGB") {
        textureformat = TextureFormat::DXT1_SRGB;
    } else if (config.TextureFormat == "DXT3_SRGB") {
        textureformat = TextureFormat::DXT3_SRGB;
    } else if (config.TextureFormat == "DXT5_SRGB") {
        textureformat = TextureFormat::DXT5_SRGB;
    } else if (config.TextureFormat == "BC4") {
        textureformat = TextureFormat::BC4;
    } else if (config.TextureFormat == "BC5") {
        textureformat = TextureFormat::BC5;
    } else {
        Engine::Error("Unknown type of texture: " + config.TextureFormat);
    }

    // Note: While having both Mirror and Clamp set on an axis is fine(Clamp takes priority), atex still strips out Mirror in case of both being set for convenience.
    if (config.Mirror_U && !config.Clamp_U) Flags |= uint32_t(TextureFlags::Mirror_U);
    if (config.Mirror_V && !config.Clamp_V) Flags |= uint32_t(TextureFlags::Mirror_V);
    if (config.Clamp_U) Flags |= uint32_t(TextureFlags::Clamp_U);
    if (config.Clamp_V) Flags |= uint32_t(TextureFlags::Clamp_V);


    // Load the texture file.
    int width, height, nrChannels;
    unsigned char* rawdata = stbi_load((inpath.parent_path()/config.Texture).c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (rawdata == nullptr) {
        Engine::Error("Failed opening file!(" + (inpath.parent_path()/config.Texture).string() + ")");
    }

    if (std::popcount((unsigned int)(width)) != 1 || std::popcount((unsigned int)(height)) != 1) {
        Engine::Error("Texture has to be a power of 2 in both dimensions.(This one is not.)");
    }

    std::vector<unsigned char> data;
    data.append_range(std::span<unsigned char>(rawdata, width * height * 4));
    stbi_image_free(rawdata);
    if (GetAmountOfChannelsFromTextureFormat(textureformat) != 4){
        data = StripTextureOfExtraData(std::move(data), textureformat);
    }

    // Set up state based on the texture format specified.
    TextureCompression* texturecompression = nullptr;
    stbir_pixel_layout stbpixellayout = STBIR_RGBA;
    stbir_datatype stbdatatype = STBIR_TYPE_UINT8;
    switch (textureformat) {
    case TextureFormat::RGBA8888:
        texturecompression = new TextureCompressionNone;
    break;
    case TextureFormat::RGBA8888_SRGB:
        texturecompression = new TextureCompressionNone;
        stbdatatype = STBIR_TYPE_UINT8_SRGB;
    break;
    case TextureFormat::DXT1:
        texturecompression = new TextureCompressionDXT1;
    break;
    case TextureFormat::DXT1_SRGB:
        texturecompression = new TextureCompressionDXT1;
        stbdatatype = STBIR_TYPE_UINT8_SRGB;
    break;
    case TextureFormat::DXT3:
        texturecompression = new TextureCompressionDXT3;
    break;
    case TextureFormat::DXT3_SRGB:
        texturecompression = new TextureCompressionDXT3;
        stbdatatype = STBIR_TYPE_UINT8_SRGB;
    break;
    case TextureFormat::DXT5:
        texturecompression = new TextureCompressionDXT5;
    break;
    case TextureFormat::DXT5_SRGB:
        texturecompression = new TextureCompressionDXT5;
        stbdatatype = STBIR_TYPE_UINT8_SRGB;
    break;
    case TextureFormat::BC4:
        texturecompression = new TextureCompressionBC4;
        stbpixellayout = STBIR_1CHANNEL;
    break;
    case TextureFormat::BC5:
        texturecompression = new TextureCompressionBC5;
        stbpixellayout = STBIR_2CHANNEL;
    break;
    default:
        Engine::Error("bruh");
    }

    // Process the texture mip levels.
    DataLump.reserve(texturecompression->PixelsToBytes(width * height) * 1.5);
    while (true) {
        // Create new mip lump entry.
        MipLump_t NewMipLump;
        NewMipLump.Width = width;
        NewMipLump.Height = height;
        NewMipLump.DataSize = texturecompression->PixelsToBytes(width * height);
        MipLump.push_back(NewMipLump);

        // Compress the data and push it into the data lump.
        DataLump.append_range(texturecompression->CompressPixels(data, width, height));

        if (config.NoMipMaps || (width == 1 && height == 1)) break;
        int newwidth = std::max(width / 2, 1);
        int newheight = std::max(height / 2, 1);

        std::vector<unsigned char> newdata;
        newdata.resize(newwidth * newheight * GetAmountOfChannelsFromTextureFormat(textureformat)); // TODO: add filter and edge settings.
        stbir_resize(data.data(), width, height, 0, newdata.data(), newwidth, newheight, 0, stbpixellayout, stbdatatype, STBIR_EDGE_WRAP, STBIR_FILTER_DEFAULT);

        data = newdata;
        width = newwidth;
        height = newheight;
    }

    Engine::BinaryFileConstructor<Texturefile> fileconstructor{1024 * 1024 * 128};
    auto file = fileconstructor.GetHeader();

    file->MipLump = fileconstructor.CreateLump<MipLump_t>(MipLump);
    file->DataLump = fileconstructor.CreateLump<char>(DataLump);
    file->Flags = Flags;
    file->Format = uint16_t(textureformat);

    std::ofstream outputfile = Engine::GetFileOutputStream(outpath, std::ios::binary);
    if (!outputfile) {
        Engine::Error("Failed to open output for writing!(" + outpath.string() + ")");
    }
    std::span<std::byte> outputspan = fileconstructor.Export();
    outputfile.write(reinterpret_cast<const char*>(outputspan.data()), outputspan.size_bytes());
    outputfile.close();

    Engine::Print("atex: Work completed!");
}