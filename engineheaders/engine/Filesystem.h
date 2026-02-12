#include <fstream>

namespace Filesystem {
    //TODO(stub)
    std::ifstream GetFile(std::string name, std::ios_base::openmode flags) {
        return std::ifstream(name, flags);
    }
}