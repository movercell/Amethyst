#pragma once

#include <glad/glad.h>
#include "engine/filesystem/Filesystem.h"
#include <map>
#include "engine/filesystem/ADF.h"


struct STDGLShaderSystem {
    struct ShaderProgram {
        bool MaterialShouldBeBoundAtDepth = false;
        GLuint Program = 0;
        GLuint DepthProgram = 0;

    protected:
        ShaderProgram(GLuint program, GLuint depthprogram, bool materialshouldbeboundatdepth) { Program = program; DepthProgram = depthprogram; MaterialShouldBeBoundAtDepth = materialshouldbeboundatdepth; }
        void Destroy() { glDeleteProgram(Program); glDeleteProgram(DepthProgram); }

        friend struct STDGLShaderSystem;
    };
private:
    std::map<std::string, GLuint> ComputeShaders;

    std::map<std::string, GLuint> VertexShaders;
    std::map<std::string, GLuint> FragmentShaders;
    std::map<std::string, GLuint> DepthShaders;
    std::map<std::string, ShaderProgram> ShaderPrograms;

    void CompileShaders(const ADFEntry& ShaderDefs, const std::string& ShaderTypeName, const GLuint ShaderType, std::map<std::string, GLuint>& OutTo, bool isRecompile);
    void CompilePrograms(const ADFEntry& ShaderDefs, bool isRecompile);

    void InitCompute(const ADFEntry& ShaderDefs, bool isRecompile);
    void InitGraphic(const ADFEntry& ShaderDefs, bool isRecompile);
    void Init_All(bool isRecompile);
public:
    void Init() { Init_All(false); };

    inline GLuint GetComputeShader(std::string name) {
        return ComputeShaders.at(name);
    } 
    // The first element of the pair is the normal version of the shader program, while the second element is the depth-only version.
    inline ShaderProgram* GetShaderProgram(const std::string& Name) {
        try {
            return &ShaderPrograms.at(Name);
        } catch(std::out_of_range e) {
            return nullptr;
        }
    };
    // Recompiles all shaders and programs.
    inline void Recompile() { Init_All(true); }

    ~STDGLShaderSystem() {
        for (auto& shader : VertexShaders) glDeleteShader(shader.second);
        for (auto& shader : FragmentShaders) glDeleteShader(shader.second);
        for (auto& shader : DepthShaders) glDeleteShader(shader.second);

        for (auto& program : ShaderPrograms) program.second.Destroy();
        for (auto& program : ComputeShaders) glDeleteProgram(program.second);
    }
};