#pragma once

#include <glad/glad.h>
#include "GLSLTypes.h"
#include "engine/filesystem/Filesystem.h"
#include "engine/filesystem/ADF.h"
#include <map>


struct STDGLShaderSystem {
    struct ShaderProgram {
        bool MaterialShouldBeBoundAtDepth = false;
        GLuint Program = 0;
        GLuint DepthProgram = 0;
        
        std140BufTemplate& GetMaterialTemplate() { return MaterialTemplate; }

    protected:
        std140BufTemplate MaterialTemplate;
        ShaderProgram(GLuint program, GLuint depthprogram, std140BufTemplate materialtemplate, bool materialshouldbeboundatdepth) { Program = program; DepthProgram = depthprogram; MaterialTemplate = std::move(materialtemplate); MaterialShouldBeBoundAtDepth = materialshouldbeboundatdepth; }
        void Destroy() { glDeleteProgram(Program); glDeleteProgram(DepthProgram); }

        friend struct STDGLShaderSystem;
    };
private:
    ADFEntry glshadersadf;

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

    inline GLuint* GetComputeShader(std::string name) {
        return &ComputeShaders.at(name);
    } 
    // The first element of the pair is the normal version of the shader program, while the second element is the depth-only version.
    inline ShaderProgram* GetShaderProgram(const std::string& Name) {
        auto ShaderProgram = ShaderPrograms.find(Name);

        if (ShaderProgram != ShaderPrograms.end()) {
            return &(ShaderProgram->second);
        } else {
            return GetShaderProgram("Engine_Error");
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