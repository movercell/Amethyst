#pragma once

#include <glad/glad.h>
#include "engine/Filesystem.h"
#include <unordered_map>

class STDGLShaderSystem {
    std::unordered_map<std::string, GLuint> ComputeShaders;
    void InitCompute(VdfParser::KeyValue Shaderdefs);
public:
    void Init();

    inline GLuint GetComputeShader(std::string name) {
        return ComputeShaders.at(name);
    }
};