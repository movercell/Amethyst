#pragma once
#include "atex.h"

struct TextureCompression {
    virtual uint64_t PixelsToBytes(uint64_t count) = 0;
    virtual std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) = 0;
};

// Uncompressed.
struct TextureCompressionNone : TextureCompression {
    uint64_t PixelsToBytes(uint64_t count) override { return count * 4; }
    std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) override { return input; }
};

struct TextureCompressionDXT1 : TextureCompression {
    uint64_t PixelsToBytes(uint64_t count) override { return std::max(count, uint64_t(16)) / 2; }
    std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) override;
};

struct TextureCompressionDXT3 : TextureCompression {
    uint64_t PixelsToBytes(uint64_t count) override { return std::max(count, uint64_t(16)); }
    std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) override;
};

struct TextureCompressionDXT5 : TextureCompression {
    uint64_t PixelsToBytes(uint64_t count) override { return std::max(count, uint64_t(16)); }
    std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) override;
};

struct TextureCompressionBC4 : TextureCompression {
    uint64_t PixelsToBytes(uint64_t count) override { return std::max(count, uint64_t(16)) / 2; }
    std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) override;
};

struct TextureCompressionBC5 : TextureCompression {
    uint64_t PixelsToBytes(uint64_t count) override { return std::max(count, uint64_t(16)); }
    std::vector<unsigned char> CompressPixels(std::vector<unsigned char>& input, int inputwidth, int inputheight) override;
};