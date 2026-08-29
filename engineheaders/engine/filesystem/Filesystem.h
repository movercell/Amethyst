#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include "engine/master.h"

namespace Filesystem {
    std::ifstream ENGINEEXPORT GetFileAsStream(const std::string& name, const std::ios_base::openmode& flags);
    std::ofstream ENGINEEXPORT GetFileOutputStream(const std::string& name, const std::ios_base::openmode& flags);
}