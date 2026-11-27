#pragma once

#define STDGLMODEL_INSTANCE_MAX_COUNT 512
#define STDGLMODEL_LOD_MAX_COUNT 4
#define STDGLMODEL_MESH_MAX_COUNT 8
#define STDGLMODEL_INSTANCE_PREPROCESS_GROUP_SIZE 128

#include <glad/glad.h>
#include "STDGLCamera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/Resource.h"
#include "GLFW/glfw3.h"
#include "IndirectDrawBuffer.h"
#include "STDGLMaterial.h"
#include "GLMisc.h"
#include <cstdint>
#include <memory>
#include <queue>
#include <map>

class STDGLModel {
    bool Make(const std::string& path, STDGLMaterialSystem* MaterialSystem);
public:
    template<bool isDepth, bool isBindless>
    void Draw() {
        glBindVertexArray(VAO);
        for (int LOD = 0; LOD < LODCount; LOD++) {
            for (int mesh = 0; mesh < LODs[LOD].MeshCount; mesh++) {
                auto& Mesh = LODs[LOD].Meshes[mesh];

                // Bind material.
                if constexpr (isDepth) {
                    if (Mesh.Material->Program->MaterialShouldBeBoundAtDepth) {
                        if constexpr (isBindless) {
                            Mesh.Material->Bind_Bindless();
                        } else {
                            Mesh.Material->Bind_Bindful();
                        }
                    }
                    glUseProgram(Mesh.Material->Program->DepthProgram);
                } else {
                    if constexpr (isBindless) {
                        Mesh.Material->Bind_Bindless();
                    } else {
                        Mesh.Material->Bind_Bindful();
                    }
                    glUseProgram(Mesh.Material->Program->Program);
                }

                // Draw the mesh.
                glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 
                    (void*)((sizeof(DrawElementsIndirectCommand) * STDGLMODEL_MESH_MAX_COUNT * LOD)
                    + sizeof(DrawElementsIndirectCommand) * mesh));
            }
        }
    }
    inline void BindInfo() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ModelInfo);
    }

    struct Mesh {
        Engine::Reference<STDGLMaterial> Material;

        Mesh() {};
    };
    struct LOD {
        uint8_t MeshCount;
        std::array<Mesh, STDGLMODEL_MESH_MAX_COUNT> Meshes;
    };
    struct ModelInfo_t {
        float Radius = 0.0f;
        std::array<float, STDGLMODEL_LOD_MAX_COUNT> LODDistances;
    };

    STDGLModel(const std::string& path, STDGLMaterialSystem* MaterialSystem);
    ~STDGLModel();

    uint8_t LODCount;
    std::array<std::array<DrawElementsIndirectCommand, STDGLMODEL_MESH_MAX_COUNT>, STDGLMODEL_LOD_MAX_COUNT> IndirectBufferTemplates;
    std::array<LOD, STDGLMODEL_LOD_MAX_COUNT> LODs;
    GLuint VAO = 0;
    GLuint VBO = 0, EBO = 0, ModelInfo = 0;
};

struct STDGLModelInstanceArray {
    struct InstanceArray {
        std::array<mat4, STDGLMODEL_INSTANCE_MAX_COUNT> InstanceMatrices;
    };
    struct InstanceArrayBuffer {
        // Must be first!
        std::array<std::array<DrawElementsIndirectCommand, STDGLMODEL_MESH_MAX_COUNT>, STDGLMODEL_LOD_MAX_COUNT> IndirectBuffers;
        
        std::array<InstanceArray, 2> Instances;
        std::array<std::array<GLuint, STDGLMODEL_INSTANCE_MAX_COUNT>, STDGLMODEL_LOD_MAX_COUNT> InstanceIndices;
    };

    GLContext* Context;
    std::queue<uint16_t> FreedIndices;
    Engine::Reference<STDGLModel> Model;
    Engine::Resource<STDGLModelInstanceArray>* selfResource;
    InstanceArray* InstanceStagingBufferMapped;
    GLuint InstanceStagingBuffer = 0;
    GLuint InstanceBuffer = 0;
    uint16_t NextIndex = 0;

    STDGLModelInstanceArray(GLContext* context, Engine::Reference<STDGLModel> model);

    ~STDGLModelInstanceArray();
        
    std::unique_ptr<ModelInstance> MakeModelInstance();
    inline void Update() {
        glFlushMappedNamedBufferRange(InstanceStagingBuffer, sizeof(InstanceArray) * Context->FrameID, NextIndex * sizeof(mat4));
        glCopyNamedBufferSubData(InstanceStagingBuffer, InstanceBuffer, sizeof(InstanceArray) * Context->FrameID, offsetof(InstanceArrayBuffer, Instances), sizeof(InstanceArray));
    }
    inline void Bind() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, InstanceBuffer);
    }
    inline void BindIndirectCommands() {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, InstanceBuffer);
    }
    inline void CopyDrawDataFromModel() {
        glNamedBufferSubData(InstanceBuffer, 0, sizeof(InstanceArrayBuffer::IndirectBuffers), &Model->IndirectBufferTemplates);
    }

};

struct STDGLModelInstance : public ModelInstance {
    Engine::Reference<STDGLModelInstanceArray> parent;
    uint16_t index;

    void SetMatrix(mat4 Matrix);

    ~STDGLModelInstance();

    STDGLModelInstance(uint16_t Index, Engine::Reference<STDGLModelInstanceArray> Parent) : index(Index), parent(Parent) {}
};

class STDGLModelSystem {
    std::map<std::string, Engine::ManagedResource<STDGLModelSystem, STDGLModel>*> Models;
    STDGLMaterialSystem* MaterialSystem = nullptr;

    template<typename Container, typename T>
    friend class Engine::ManagedResource;
    void _unmanage_resource(Engine::Resource<STDGLModel>* res) {
        Engine::Print("hi");
        for (auto it = Models.begin(); it != Models.end(); ++it) {
            if (it->second == res) {
                Models.erase(it);
                break;
            }
        }
        delete res;
    }
public:
    void Init(STDGLMaterialSystem* materialsystem) { MaterialSystem = materialsystem; }
    Engine::Reference<STDGLModel> GetModel(const std::string& path);

};