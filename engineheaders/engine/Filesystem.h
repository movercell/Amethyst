#pragma once

#include <fstream>
#include <sstream>
#include "vdfparser/vdfparser.hpp"
#include <iostream>

//TODO(stub)
namespace Filesystem {
    inline std::ifstream GetFile(const std::string& name, const std::ios_base::openmode& flags) {
        return std::ifstream(name, flags);
    }
    inline VdfParser::KeyValue GetDataFile(const std::string& name) {
        std::ifstream file(name);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << name << std::endl;
            exit(-1);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return VdfParser::fromString(buffer.str());
    }
}