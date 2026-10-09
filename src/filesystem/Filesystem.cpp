#include "engine/filesystem/Filesystem.h"
#include "engine/master.h"

// TODO:: stub

std::ifstream Filesystem::GetFileAsStream(const std::string& name, const std::ios_base::openmode& flags) {
        std::filesystem::path path = name;
        if (std::filesystem::is_directory(path)) {
                Engine::Print("Attempted to read from path \"" + name + "\", which is a directory!");
                return std::ifstream();
        }
        return std::ifstream(path, flags);
}

std::ofstream Filesystem::GetFileOutputStream(const std::string& name, const std::ios_base::openmode& flags) {
        std::filesystem::path path = name;
        if (std::filesystem::is_directory(path)) {
                Engine::Print("Attempted to write to path \"" + name + "\", which is a directory!");
                return std::ofstream();
        }
        return std::ofstream(path, flags);
}

std::filesystem::path ENGINEEXPORT Filesystem::GetGameDirectoryPath() {
        return "./";
}