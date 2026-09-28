#include "engine/filesystem/Filesystem.h"
#include "engine/master.h"

// TODO:: stub

std::ifstream Filesystem::GetFileAsStream(const std::string& name, const std::ios_base::openmode& flags) {
        return std::ifstream(name, flags);
}

std::ofstream Filesystem::GetFileOutputStream(const std::string& name, const std::ios_base::openmode& flags) {
        return std::ofstream(name, flags);
}

std::filesystem::path ENGINEEXPORT GetGameDirectoryPath() {
        return "./";
}