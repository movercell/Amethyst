#include "STDGLShaderSystem.h"
#include "Shadinclude.hpp"

void STDGLShaderSystem::InitCompute(VdfParser::KeyValue Shaderdefs) {
    if (!Shaderdefs.getChildren().has_value()) return;

    auto ShaderDefList = Shaderdefs.getChildren().value();
    for (const auto& shader : ShaderDefList) {
        auto Shaderpath = shader.second.getChild("Source");
        if (!Shaderpath.has_value()) continue;
        std::string ShaderSrc = Shadinclude::load("scripts/shaders/opengl/" + std::get<std::string>(Shaderpath->value)); 
        
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
        auto ShaderSrc_Cstr = ShaderSrc.c_str();
        glShaderSource(computeShader, 1, &ShaderSrc_Cstr, NULL);

        glCompileShader(computeShader);

        int success;
        char infoLog[512];
        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
            std::cout << infoLog << std::endl;
            exit(-1);
        }

        GLuint computeProgram = glCreateProgram();
        glAttachShader(computeProgram, computeShader);
        glLinkProgram(computeProgram);

        glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(computeProgram, 512, NULL, infoLog);
            std::cout << infoLog << std::endl;
            exit(-1);
        }

    }
}


void STDGLShaderSystem::Init() {
    auto glshadersadf = Filesystem::GetDataFile("scripts/shaders/glshaders.adf").getChild("Shaders").value();

    auto ComputeShaderDefs = glshadersadf.getChild("Compute");
    if (ComputeShaderDefs.has_value()) {
        InitCompute(ComputeShaderDefs.value());
    }
}