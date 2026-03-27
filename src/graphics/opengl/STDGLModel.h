#pragma once

#define STDGLMODEL_INSTANCE_MAX_COUNT 4096
#define STDGLMODEL_LOD_MAX_COUNT 1
#define STDGLMODEL_MESH_MAX_COUNT 8

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
        for (int mesh = 0; mesh < MeshCount; mesh++) {
            glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(sizeof(DrawElementsIndirectCommand) * mesh));
        }
    }
    void DrawDepth();

    struct Mesh {
        unsigned int IndexCount;
        unsigned int BaseVertex;
        unsigned int BaseIndex;

        Mesh() {};
        Mesh(unsigned int indexcount, unsigned int basevertex, unsigned int baseindex) : IndexCount(indexcount), BaseVertex(basevertex), BaseIndex(baseindex) {}
    };
    struct ModelInfo_t {
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
        DrawElementsIndirectCommand IndirectBuffers[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT]; 
        mat4 InstanceMatrices[STDGLMODEL_INSTANCE_MAX_COUNT];
        GLuint InstanceIndeces[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_INSTANCE_MAX_COUNT];
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