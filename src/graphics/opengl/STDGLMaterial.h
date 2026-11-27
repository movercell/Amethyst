#pragma once

#include <map>
#include <string>
#include "engine/Resource.h"
#include "STDGLShaderSystem.h"
#include "glad/glad.h"

class STDGLMaterialSystem;

class STDGLMaterial {
    std140Buf Descriptor;
    GLuint ubo = 0;
    std::map<std::string, Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>*>::iterator IteratorInMaterialMap;

    friend class STDGLMaterialSystem;
public:
    STDGLMaterial() {};
    ~STDGLMaterial() { glDeleteBuffers(1, &ubo); }
    
    STDGLShaderSystem::ShaderProgram* Program;

    void Bind_Bindful();
    void Bind_Bindless();
};

class STDGLMaterialSystem {
    STDGLShaderSystem* ShaderSystem = nullptr;
    std::map<std::string, Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>*> Materials;
    Engine::Reference<STDGLMaterial> DefaultMaterial; // Must come after the map due to destruction order.
    STDGLTextureSystem* TextureSystem;

    void MakeMaterialData(Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>* Material, std::string& name);
public:
    STDGLMaterialSystem() {};

    void Init(STDGLShaderSystem* shadersystem, STDGLTextureSystem* texturesystem);
    void ReInit(STDGLTextureSystem* texturesystem);
    Engine::Reference<STDGLMaterial> Get(std::string name);

    void _unmanage_resource(Engine::Resource<STDGLMaterial>* res) {
        auto* mat = res->Get();
        Materials.erase(mat->IteratorInMaterialMap);
        delete res;
    }
};