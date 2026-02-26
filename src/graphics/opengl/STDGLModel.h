#pragma once

#define STDGLMODEL_INSTANCE_MAX_COUNT 4096
#define STDGLMODEL_MESH_MAX_COUNT 8

#include "STDGLCamera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/geometry/Model.h"
#include "GLFW/glfw3.h"
#include <cstdint>
#include <memory>
#include <queue>

class STDGLModel {
public:
    void Draw();
    void DrawDepth();

    struct Mesh {
        unsigned int IndexCount;
        unsigned int BaseVertex;
        unsigned int BaseIndex;

        Mesh(unsigned int indexcount, unsigned int basevertex, unsigned int baseindex) : IndexCount(indexcount), BaseVertex(basevertex), BaseIndex(baseindex) {}
    };
    struct ModelInfo_t {
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
    uint16_t InstanceMaxCount; // Used to limit the amount of static model instances to just what's needed.
    std::shared_ptr<STDGLModel> Model;
    std::weak_ptr<STDGLModelInstanceArray> selfRef;

    STDGLModelInstanceArray(GLFWwindow* data, std::shared_ptr<STDGLModel> model, uint16_t instancemaxcount = STDGLMODEL_INSTANCE_MAX_COUNT);

    ~STDGLModelInstanceArray();
        
    std::unique_ptr<ModelInstance> MakeModelInstance();
    void Draw();

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