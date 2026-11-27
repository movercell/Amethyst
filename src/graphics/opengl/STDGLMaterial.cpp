#include "STDGLMaterial.h"

struct ADFSerialize MaterialEntry {
    std::string Program;
    std::map<std::string, ADFEntry> Parameters;
};

void STDGLMaterialSystem::Init(STDGLShaderSystem* shadersystem, STDGLTextureSystem* texturesystem) {
    ShaderSystem = shadersystem;
    TextureSystem = texturesystem;
    DefaultMaterial = Get("materials/error.amat");
}

void STDGLMaterialSystem::ReInit(STDGLTextureSystem* texturesystem) {
    TextureSystem = texturesystem;

    for (auto& matpair : Materials) {
        STDGLMaterial& mat = matpair.second->resource;
        glDeleteBuffers(1, &mat.ubo);
        auto name = std::string(matpair.first);
        MakeMaterialData(matpair.second, name);
    }
}

void STDGLMaterialSystem::MakeMaterialData(Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>* Material, std::string& name) {
    MaterialEntry Params;
    {
        auto tmp = ADFEntry::FromFile(name);
        if (!tmp.HasChild("Material")) {
            tmp = ADFEntry::FromFile("materials/error.amat");
        }
        tmp["Material"].Deserialize(Params);
    }
    STDGLShaderSystem::ShaderProgram* Program = ShaderSystem->GetShaderProgram(Params.Program);
    auto& Template = Program->GetMaterialTemplate();
    Material->resource.Program = Program;
    auto buffer = Template.CreateBuffer();
    for (auto& Param : Params.Parameters) {
        Template.BufferSetValue(buffer, Param.first, Param.second, TextureSystem);
    }
    glCreateBuffers(1, &Material->resource.ubo);
    glNamedBufferStorage(Material->resource.ubo, std::max(Template.GetBufferSize(), size_t(1)), buffer.buffer.get(), 0);

    Material->resource.Descriptor = std::move(buffer);
}

Engine::Reference<STDGLMaterial> STDGLMaterialSystem::Get(std::string name) {
    {
        auto it = Materials.find(name);
        if (it != Materials.end()) {
            return it->second;
        }
    }

    auto* Material = new Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>(this);
    MakeMaterialData(Material, name);
    Material->resource.IteratorInMaterialMap = Materials.emplace(std::move(name), Material).first;

    return Material;
}

void STDGLMaterial::Bind_Bindful() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
    for (int tex = 0; tex < Descriptor.textures.size(); tex++) {
        glBindTextureUnit(tex, Descriptor.textures[tex]);
    }
}
void STDGLMaterial::Bind_Bindless() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
}