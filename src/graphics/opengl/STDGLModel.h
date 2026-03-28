#pragma once

#define STDGLMODEL_INSTANCE_MAX_COUNT 4096
#define STDGLMODEL_LOD_MAX_COUNT 1
#define STDGLMODEL_MESH_MAX_COUNT 8
#define STDGLMODEL_INSTANCE_PREPROCESS_GROUP_SIZE 128

#include <glad/glad.h>
#include "STDGLCamera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/geometry/Model.h"
#include "GLFW/glfw3.h"
#include "InderectDrawBuffer.h"
#include <cstdint>
#include <memory>
#include <queue>

class STDGLModel {
public:
    void Draw() {
        glBindVertexArray(VAO);
        for (int mesh = 0; mesh < MeshCount; mesh++) {
            glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(sizeof(DrawElementsIndirectCommand) * mesh));
        }
    }
    void DrawDepth();
    inline void BindInfo() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ModelInfo);
    }
    inline void BindIndirectCommands() {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ModelInfo);
    }

    struct Mesh {
        unsigned int stub;

        Mesh() {};
    };
    struct ModelInfo_t {
        DrawElementsIndirectCommand IndirectBuffers[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT]; 
        GLuint InstanceIndeces[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_INSTANCE_MAX_COUNT];
        float Radius = 0.0f;
    };

    STDGLModel(std::string path);
    ~STDGLModel();

    uint8_t LODCount;
    uint8_t MeshCount;
    Mesh Meshes[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT];
    std::string Path;
    GLuint VAO;
    GLuint VBO, EBO, ModelInfo;
};

class STDGLModelInstanceArray {
public:
    struct InstanceArrayBuffer {
        mat4 InstanceMatrices[STDGLMODEL_INSTANCE_MAX_COUNT];
    };

    GLFWwindow* rendererData;
    GLuint InstanceBuffer = 0;
    std::queue<uint16_t> FreedIndeces;
    uint16_t NextIndex = 0;
    std::shared_ptr<STDGLModel> Model;
    std::weak_ptr<STDGLModelInstanceArray> selfRef;
    InstanceArrayBuffer* InstanceBufferMapped;

    STDGLModelInstanceArray(GLFWwindow* data, std::shared_ptr<STDGLModel> model);

    ~STDGLModelInstanceArray();
        
    std::unique_ptr<ModelInstance> MakeModelInstance();
    inline void Bind() {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, InstanceBuffer,
                        sizeof(InstanceArrayBuffer) * (Engine::FrameCount & 1),
                        sizeof(InstanceArrayBuffer));
    }

    inline void Flush() {
        glFlushMappedNamedBufferRange(InstanceBuffer, 
                    sizeof(InstanceArrayBuffer) * (Engine::FrameCount & 1),
                    sizeof(InstanceArrayBuffer));
    }

    friend class GLModelInstance;
};

class STDGLModelInstance : public ModelInstance {
public:
    std::shared_ptr<STDGLModelInstanceArray> parent;
    uint16_t index;

    void SetMatrix(mat4 Matrix);

    ~STDGLModelInstance();

    STDGLModelInstance(uint16_t Index, std::shared_ptr<STDGLModelInstanceArray> Parent) { index = Index; parent = Parent; }
};


class STDGLModelSystem {
public:
    weak_vector<STDGLModel> Models;

    std::shared_ptr<STDGLModel> GetModel(std::string path);
};