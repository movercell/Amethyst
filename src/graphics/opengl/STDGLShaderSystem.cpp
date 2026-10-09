#include "STDGLShaderSystem.h"
#include "Shadinclude.hpp"
#include "engine/master.h"


void STDGLShaderSystem::InitCompute(const ADFEntry& ShaderDefs, bool isRecompile) {
    const auto& ShaderDefMap = ShaderDefs["Compute"].GetMap();
    if (ShaderDefMap.empty()) return;

    for (const auto& shader : ShaderDefMap) {
        if (!shader.second.HasChild("Source")) {
            Engine::Warning("Shader " + shader.first + " does not have a \"Source\", ignoring!");
            continue;
        }
        auto Shaderpath = shader.second["Source"];
        std::string ShaderSrc = Shadinclude::load("scripts/shaders/opengl/" + Shaderpath.GetString()); 
        
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
        auto ShaderSrc_Cstr = ShaderSrc.c_str();
        glShaderSource(computeShader, 1, &ShaderSrc_Cstr, NULL);

        glCompileShader(computeShader);

        int success;
        char infoLog[512];
        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
            std::cout << Shaderpath.GetString() << std::endl;
            std::cout << infoLog << std::endl;
            Engine::Warning("Compute shader compilation error! Log printed to std::cout");
            glDeleteShader(computeShader);
            computeShader = 0;
        }

        GLuint computeProgram = glCreateProgram();
        glAttachShader(computeProgram, computeShader);
        glLinkProgram(computeProgram);

        glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(computeProgram, 512, NULL, infoLog);
            std::cout << Shaderpath.GetString() << std::endl;
            std::cout << infoLog << std::endl;
            Engine::Warning("Compute shader program linking error! Log printed to std::cout");
            glDeleteProgram(computeProgram);
            computeProgram = 0;
        }

        if (isRecompile) {
            if (computeProgram == 0)
                return; // No need to replace a working program with null

            glDeleteProgram(ComputeShaders.at(shader.first));
            ComputeShaders.at(shader.first) = computeProgram;
        } else {
            ComputeShaders.emplace(shader.first, computeProgram);
        }
    }
}

void STDGLShaderSystem::CompileShaders(const ADFEntry& ShaderDefs, const std::string& ShaderTypeName, const GLuint ShaderType, std::map<std::string, GLuint>& OutTo, bool isRecompile) {
    const auto& ShaderDefMap = ShaderDefs[ShaderTypeName].GetMap();

    for (const auto& shader : ShaderDefMap) {
        if (!shader.second.HasChild("Source")) {
            Engine::Error(ShaderTypeName + " shader " + shader.first + " does not have a \"Source\"!");
        }
        auto Shaderpath = shader.second["Source"];
        std::string ShaderSrc = Shadinclude::load("scripts/shaders/opengl/" + Shaderpath.GetString()); 
        
        GLuint Shader = glCreateShader(ShaderType);
        auto ShaderSrc_Cstr = ShaderSrc.c_str();
        int ShaderSrcLength = ShaderSrc.length();
        glShaderSource(Shader, 1, &ShaderSrc_Cstr, &ShaderSrcLength);
        glCompileShader(Shader);

        int success;
        char infoLog[512];
        glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(Shader, 512, NULL, infoLog);
            Engine::Warning(ShaderTypeName + " shader compilation error!\nIn file:" + Shaderpath.GetString() + '\n' + infoLog);
            glDeleteShader(Shader);
            Shader = 0;
        }

        if (isRecompile) {
            if (Shader == 0)
                return; // No need to replace a working shader with null

            glDeleteShader(OutTo.at(shader.first));
            OutTo.at(shader.first) = Shader;
        } else {
            OutTo.emplace(shader.first, Shader);
        }
    }
}

void STDGLShaderSystem::CompilePrograms(const ADFEntry& ShaderDefs, bool isRecompile) {
    const auto& ProgramDefs = ShaderDefs["Programs"].GetMap();
    for (const auto& program : ProgramDefs) {
        const auto& programmap = program.second.GetMap();
        GLuint vert  = VertexShaders.at(programmap.at("Shader_Vertex").GetString());
        GLuint frag  = FragmentShaders.at(programmap.at("Shader_Fragment").GetString());
        GLuint depth = DepthShaders.at(programmap.at("Shader_Depth").GetString());

        GLuint Program = glCreateProgram();
        glAttachShader(Program, vert);
        glAttachShader(Program, frag);
        glLinkProgram(Program);

        int success;
        char infoLog[512];
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(Program, 512, NULL, infoLog);
            Engine::Warning("Program link error!\nIn program:" + program.first + '\n' + infoLog);
            glDeleteProgram(Program);
            Program = 0;
        }

        GLuint DepthProgram = glCreateProgram();
        glAttachShader(DepthProgram, vert);
        glAttachShader(DepthProgram, depth);
        glLinkProgram(DepthProgram);
        
        glGetProgramiv(DepthProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(DepthProgram, 512, NULL, infoLog);
            Engine::Warning("(Depth)Program link error!\nIn program:" + program.first + '\n' + infoLog);
            glDeleteProgram(DepthProgram);
            DepthProgram = 0;
        }

        bool MaterialShouldBeBoundAtDepth = false;
        if (programmap.contains("MaterialShouldBeBoundAtDepth")) MaterialShouldBeBoundAtDepth = programmap.at("MaterialShouldBeBoundAtDepth").GetString() == "1" ? true : false;


        // Handle the material layout.
        std140BufTemplate MaterialTemplate;
        if (programmap.contains("MaterialLayout")) {
            const ADFEntry& MaterialLayout = programmap.at("MaterialLayout");
            for (const ADFEntry& member : MaterialLayout.GetArray()) {
                std::optional<ADFEntry> defaultvalue;
                if (member.HasChild("Default")) {
                    defaultvalue = member["Default"];
                }

                MaterialTemplate.AddMember(member["Name"].GetString(), member["Type"].GetString(), defaultvalue);
            }
        }
        MaterialTemplate.Shrink();

        ShaderProgram ShaderProgramObject = ShaderProgram(Program, DepthProgram, std::move(MaterialTemplate), MaterialShouldBeBoundAtDepth);
        if (isRecompile) {
            if (Program == 0 || DepthProgram == 0)
                return; // No need to replace a working program with null

            ShaderPrograms.at(program.first).Destroy();
            ShaderPrograms.at(program.first) = std::move(ShaderProgramObject);
        } else {
            ShaderPrograms.emplace(program.first, std::move(ShaderProgramObject));
        }
    }
}

void STDGLShaderSystem::InitGraphic(const ADFEntry& ShaderDefs, bool isRecompile) {
    CompileShaders(ShaderDefs, "Vertex", GL_VERTEX_SHADER, VertexShaders, isRecompile);
    CompileShaders(ShaderDefs, "Fragment", GL_FRAGMENT_SHADER, FragmentShaders, isRecompile);
    CompileShaders(ShaderDefs, "Depth", GL_FRAGMENT_SHADER, DepthShaders, isRecompile);

    CompilePrograms(ShaderDefs, isRecompile);
}


void STDGLShaderSystem::Init_All(bool isRecompile) {
    glshadersadf = ADFEntry::FromFile("scripts/shaders/glshaders.adf")["Shaders"];

    InitCompute(glshadersadf, isRecompile);
    InitGraphic(glshadersadf, isRecompile);
}
