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
    void Bind();
    void Draw();
    void DrawDepth();

    struct Mesh {
        unsigned int IndexCount;
        unsigned int BaseVertex;
        unsigned int BaseIndex;

        Mesh(unsigned int indexcount, unsigned int basevertex, unsigned int baseindex) : IndexCount(indexcount), BaseVertex(basevertex), BaseIndex(baseindex) {}
    };
    struct ModelInfo_t {
        DrawElementsIndirectCommandSTD140 IndirectBufferTemplates[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT];
        float Radius = 0.0f;
    };

    STDGLModel(std::string path);
    ~STDGLModel();

    std::vector<Mesh> Meshes;
    std::string Path;
    GLuint VAO;
    GLuint VBO, EBO, ModelInfo;
};

class STDGLModelInstanceArray {
public:
    GLFWwindow* rendererData;
    GLuint InstanceBuffer = 0;
    std::queue<uint16_t> FreedIndeces;
    uint16_t NextIndex = 0;
    std::shared_ptr<STDGLModel> Model;
    std::weak_ptr<STDGLModelInstanceArray> selfRef;

    struct InstanceArrayBuffer {
        DrawElementsIndirectCommand IndirectBuffers[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT]; 
        char InstanceMatrices[STDGLMODEL_INSTANCE_MAX_COUNT * sizeof(mat4)];
    };

    STDGLModelInstanceArray(GLFWwindow* data, std::shared_ptr<STDGLModel> model);

    ~STDGLModelInstanceArray();
        
    std::unique_ptr<ModelInstance> MakeModelInstance();
    void Bind();

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