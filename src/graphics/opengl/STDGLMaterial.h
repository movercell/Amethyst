#pragma once

#include <map>
#include <string>
#include "engine/Resource.h"
#include "STDGLShaderSystem.h"
#include "glad/glad.h"

class STDGLMaterialSystem;

class STDGLMaterial {
    GLuint ubo = 0;
    std::map<std::string, Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>*>::iterator IteratorInMaterialMap;

    friend class STDGLMaterialSystem;
public:
    STDGLMaterial() {};
    
    STDGLShaderSystem::ShaderProgram* Program;

    void Bind();
};

class STDGLMaterialSystem {
    STDGLShaderSystem* ShaderSystem = nullptr;
    Engine::Reference<STDGLMaterial> DefaultMaterial;
    std::map<std::string, Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>*> Materials;
public:
    STDGLMaterialSystem() {};

    void Init(STDGLShaderSystem* shadersystem);
    Engine::Reference<STDGLMaterial> Get(std::string name);

    void _unmanage_resource(Engine::Resource<STDGLMaterial>* res) {
        auto* mat = res->Get();
        glDeleteBuffers(1, &mat->ubo);
        Materials.erase(mat->IteratorInMaterialMap);
    }
};