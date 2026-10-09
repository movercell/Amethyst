#include "STDGLMaterial.h"

struct ADFSerialize MaterialEntry {
    std::string Program;
    std::map<std::string, ADFEntry> Parameters;
};

void STDGLMaterialSystem::Init(STDGLShaderSystem* shadersystem) {
    ShaderSystem = shadersystem;
    DefaultMaterial = Get("materials/error.amt");
}

Engine::Reference<STDGLMaterial> STDGLMaterialSystem::Get(std::string name) {
    {
        auto it = Materials.find(name);
        if (it != Materials.end()) {
            return it->second;
        }
    }

    auto* Material = new Engine::ManagedResource<STDGLMaterialSystem, STDGLMaterial>(this);

    MaterialEntry Params;
    {
        auto tmp = ADFEntry::FromFile(name);
        if (!tmp.HasChild("Material")) {
            tmp = ADFEntry::FromFile("materials/error.amt");
        }
        tmp["Material"].Deserialize(Params);
    }
    STDGLShaderSystem::ShaderProgram* Program = ShaderSystem->GetShaderProgram(Params.Program);
    auto& Template = Program->GetMaterialTemplate();
    Material->resource.Program = Program;
    auto buffer = Template.CreateBuffer();
    for (auto& Param : Params.Parameters) {
        Template.BufferSetValue(buffer, Param.first, Param.second);
    }
    glCreateBuffers(1, &Material->resource.ubo);
    glNamedBufferStorage(Material->resource.ubo, Template.GetBufferSize(), buffer.get(), 0);

    Material->resource.IteratorInMaterialMap = Materials.emplace(std::move(name), Material).first;

    return Material;
}

void STDGLMaterial::Bind() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
}