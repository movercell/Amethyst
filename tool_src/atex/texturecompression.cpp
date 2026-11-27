#include "texturecompression.h"
#include "stb_dxt.h"

template<int blocksize, typename pixel_t>
struct BasicBlockCompressor {
    std::vector<unsigned char>& texture;
    int texwidth = 0;
    int texheight = 0;
    int currx = 0;
    int curry = 0;

    BasicBlockCompressor(std::vector<unsigned char>& input, int width, int height) : texture(input), texwidth(width), texheight(height) {}
    operator bool() {
        return currx < texwidth && curry < texheight;
    }

    auto GetBlock() {
        std::array<std::array<pixel_t, blocksize>, blocksize> ret;
        for (int x = 0; x < blocksize; x++) {
            for (int y = 0; y < blocksize; y++) {
                uint64_t pixeloffset = std::min(currx + x, texwidth - 1) + (std::min(curry + y, texheight - 1) * texwidth);
                ret[y][x] = reinterpret_cast<pixel_t*>(texture.data())[pixeloffset];
            }
        }

        // Progress the block.
        currx += blocksize;
        if (currx == texwidth) {
            currx = 0;
            curry += blocksize;
        }

        return ret;
    }

};

std::vector<unsigned char> TextureCompressionDXT1::CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) {
    BasicBlockCompressor<4, uint32_t> layout = {input, inputwidth, inputheight};
    std::vector<unsigned char> newdata;
    newdata.reserve(PixelsToBytes(inputwidth * inputheight));

    while (layout) {
        auto block = layout.GetBlock();
        std::array<unsigned char, 8> compressedblock;

        stb_compress_dxt_block(compressedblock.data(), reinterpret_cast<unsigned char*>(block.data()), 0, STB_DXT_HIGHQUAL);

        newdata.insert(newdata.end(), compressedblock.begin(), compressedblock.end());
    }

    return newdata;
}

std::vector<unsigned char> TextureCompressionDXT3::CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) {
    BasicBlockCompressor<4, uint32_t> layout = {input, inputwidth, inputheight};
    std::vector<unsigned char> newdata;
    newdata.reserve(PixelsToBytes(inputwidth * inputheight));

    while (layout) {
        auto block = layout.GetBlock();
        std::array<unsigned char, 16> compressedblock;

        int alphaindex = 0;

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x += 2) {
                unsigned char b1 = block[y][x] >> 24;
                unsigned char b2 = block[y][x + 1] >> 24;
                unsigned char alpha =
                    ((b1 + 8) / 17) << 4 |
                    ((b2 + 8) / 17);
                compressedblock[alphaindex] = alpha;
                alphaindex++;
            }
        }

        stb_compress_dxt_block(compressedblock.data() + 8, reinterpret_cast<unsigned char*>(block.data()), 0, STB_DXT_HIGHQUAL);

        newdata.insert(newdata.end(), compressedblock.begin(), compressedblock.end());
    }

    return newdata;
}

std::vector<unsigned char> TextureCompressionDXT5::CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) {
    BasicBlockCompressor<4, uint32_t> layout = {input, inputwidth, inputheight};
    std::vector<unsigned char> newdata;
    newdata.reserve(PixelsToBytes(inputwidth * inputheight));

    while (layout) {
        auto block = layout.GetBlock();
        std::array<unsigned char, 16> compressedblock;

        stb_compress_dxt_block(compressedblock.data(), reinterpret_cast<unsigned char*>(block.data()), 1, STB_DXT_HIGHQUAL);

        newdata.insert(newdata.end(), compressedblock.begin(), compressedblock.end());
    }

    return newdata;
}

std::vector<unsigned char> TextureCompressionBC4::CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) {
    BasicBlockCompressor<4, uint8_t> layout = {input, inputwidth, inputheight};
    std::vector<unsigned char> newdata;
    newdata.reserve(PixelsToBytes(inputwidth * inputheight));

    while (layout) {
        auto block = layout.GetBlock();
        std::array<unsigned char, 8> compressedblock;

        stb_compress_bc4_block(compressedblock.data(), reinterpret_cast<unsigned char*>(block.data()));

        newdata.insert(newdata.end(), compressedblock.begin(), compressedblock.end());
    }

    return newdata;
}

std::vector<unsigned char> TextureCompressionBC5::CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) {
    BasicBlockCompressor<4, uint16_t> layout = {input, inputwidth, inputheight};
    std::vector<unsigned char> newdata;
    newdata.reserve(PixelsToBytes(inputwidth * inputheight));

    while (layout) {
        auto block = layout.GetBlock();
        std::array<unsigned char, 16> compressedblock;

        stb_compress_bc5_block(compressedblock.data(), reinterpret_cast<unsigned char*>(block.data()));

        newdata.insert(newdata.end(), compressedblock.begin(), compressedblock.end());
    }

    return newdata;
}