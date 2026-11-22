#include "engine/Binaryfile.h"
#include <fstream>

struct BinaryFile("ATEX", 0) TexFile : Engine::BinaryFileBaseHeader {
    Engine::BinaryFileLump<std::byte> TexLump;
};
struct BinaryFile("ATEX", 1) TexFileV1 : TexFile {};

using strlmp = Engine::BinaryFileStringLump;

int main() {
    Engine::BinaryFileConstructor<TexFileV1> constructor;
    TexFile* file = constructor.GetHeader();

    std::array<std::byte, 16> texture = {std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('a'), std::byte('\0')};
    file->TexLump = {constructor.CreateLump<std::byte>(texture)};

    auto hi = constructor.Export();
    //hi[5] = std::byte(0xFF);

    std::cout << "hi1" << std::endl;

    std::ofstream outFile("test.atex", std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(hi.data()), hi.size_bytes());
    outFile.close();

    auto NewFile = Engine::BinaryFileImporter<TexFile, TexFileV1>::Import(hi, "test");

    std::cout << "hi2" << std::endl;
}