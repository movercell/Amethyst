#include "STDGLShaderSystem.h"
#include "Shadinclude.hpp"
#include "engine/master.h"

void STDGLShaderSystem::InitCompute(const ADFEntry& ShaderDefs) {
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
            continue;
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
            continue;
        }

        ComputeShaders.emplace(shader.first, ComputeShader(computeProgram, Shaderpath.GetString()));

    }
}

void STDGLShaderSystem::InitGraphic(const ADFEntry& ShaderDefs) {
    // Vertex shaders.
    const auto& VertexShaderDefMap = ShaderDefs["Vertex"].GetMap();
    VertexShaders.reserve(VertexShaderDefMap.size());

    for (const auto& shader : VertexShaderDefMap) {
        if (!shader.second.HasChild("Source")) {
            Engine::Warning("Vertex shader " + shader.first + " does not have a \"Source\", ignoring!");
            continue;
        }
        auto Shaderpath = shader.second["Source"];
        std::string ShaderSrc = Shadinclude::load("scripts/shaders/opengl/" + Shaderpath.GetString()); 
        
        auto ShaderSrc_Cstr = ShaderSrc.c_str();
        GLuint vertexShader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &ShaderSrc_Cstr);

        int success;
        char infoLog[512];
        glGetProgramiv(vertexShader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << Shaderpath.GetString() << std::endl;
            std::cout << infoLog << std::endl;
            Engine::Warning("Vertex shader compilation error! Log printed to std::cout");
            glDeleteShader(vertexShader);
            continue;
        }

        VertexShaders.emplace_back(vertexShader, Shaderpath.GetString());
        VertexShaderNameToIndex.emplace(shader.first, VertexShaders.size() - 1);
    }



    // Fragment + Depth shaders.
    const auto& FragmentShaderDefMap = ShaderDefs["Fragment"].GetMap();
    FragmentShaders.reserve(FragmentShaderDefMap.size());

    for (const auto& shader : FragmentShaderDefMap) {
        // First, fragment.
        {
            if (!shader.second.HasChild("Source")) {
                Engine::Warning("Fragment shader " + shader.first + " does not have a \"Source\", ignoring!");
                continue;
            }
            auto Shaderpath = shader.second["Source"];
            std::string ShaderSrc = Shadinclude::load("scripts/shaders/opengl/" + Shaderpath.GetString()); 

            auto ShaderSrc_Cstr = ShaderSrc.c_str();
            GLuint fragmentShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &ShaderSrc_Cstr);

            int success;
            char infoLog[512];
            glGetProgramiv(fragmentShader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << Shaderpath.GetString() << std::endl;
                std::cout << infoLog << std::endl;
                std::cout << Shaderpath.GetString() << std::endl;
                Engine::Warning("Fragment shader compilation error! Log printed to std::cout");
                glDeleteShader(fragmentShader);
                continue;
            }

            FragmentShaders.emplace_back(fragmentShader, Shaderpath.GetString());
            FragmentShaderNameToIndex.emplace(shader.first, FragmentShaders.size() - 1);

        }


        // Then, depth.
        {
            ADFEntry Shaderpath = ADFEntry::String("generic.ds");
            if (shader.second.HasChild("DepthSource")) {
                Shaderpath = shader.second["DepthSource"];
            }
            std::string ShaderSrc = Shadinclude::load("scripts/shaders/opengl/" + Shaderpath.GetString()); 

            auto ShaderSrc_Cstr = ShaderSrc.c_str();
            GLuint depthShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &ShaderSrc_Cstr);

            int success;
            char infoLog[512];
            glGetProgramiv(depthShader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(depthShader, 512, NULL, infoLog);
                std::cout << Shaderpath.GetString() << std::endl;
                std::cout << infoLog << std::endl;
                Engine::Warning("Depth shader compilation error! Log printed to std::cout");
                glDeleteShader(depthShader);
                // Since the fragment shader entry already exists it means that it needs to just skip the current depth shader and add the null shader to the list.
                depthShader = 0;
            }

            DepthShaders.emplace_back(depthShader, Shaderpath.GetString());
        }
    }
}


void STDGLShaderSystem::Init() {
    auto glshadersadf = ADFEntry::FromFile("scripts/shaders/glshaders.adf")["Shaders"];

    InitCompute(glshadersadf);
    InitGraphic(glshadersadf);
}



std::pair<GLuint, GLuint> STDGLShaderSystem::GetShaderPipeline(const std::string& VertexName, const std::string& FragmentName) {
    uint16_t VertexID;
    uint16_t FragmentID;

    try {
        VertexID = VertexShaderNameToIndex.at(VertexName);
    } catch(...) {
        Engine::Warning("Attempted to use a non-existent vertex shader with the name of: \"" + VertexName + "\"");
        return {0, 0};
    }
    try {
        FragmentID = FragmentShaderNameToIndex.at(FragmentName);
    } catch(...) {
        Engine::Warning("Attempted to use a non-existent fragment shader with the name of: \"" + FragmentName + "\"");
        return {0, 0};
    }


    ShaderPipelineKey Key      = ShaderPipelineKey(VertexID, FragmentID, false);
    ShaderPipelineKey DepthKey = ShaderPipelineKey(VertexID, FragmentID, true);
    try {
        return {CachedShaderPipelines.at(Key.KeyAsInt),
                CachedShaderPipelines.at(DepthKey.KeyAsInt)};
    } catch(...) {
        // Pipeline not cached, have to create it.
        GLuint VertexShader = VertexShaders[VertexID].ShaderObject;
        GLuint FragmentShader = FragmentShaders[VertexID].ShaderObject;
        GLuint DepthShader = DepthShaders[VertexID].ShaderObject;

        GLuint PipelineID;
        glCreateProgramPipelines(1, &PipelineID);
        glUseProgramStages(PipelineID, GL_VERTEX_SHADER_BIT, VertexShader);
        glUseProgramStages(PipelineID, GL_FRAGMENT_SHADER_BIT, FragmentShader);
        CachedShaderPipelines.emplace(Key.KeyAsInt, PipelineID);

        GLuint DepthPipelineID;
        glCreateProgramPipelines(1, &DepthPipelineID);
        glUseProgramStages(DepthPipelineID, GL_VERTEX_SHADER_BIT, VertexShader);
        glUseProgramStages(DepthPipelineID, GL_FRAGMENT_SHADER_BIT, DepthShader);
        CachedShaderPipelines.emplace(DepthKey.KeyAsInt, DepthPipelineID);

        return {PipelineID, DepthPipelineID};
    }
}