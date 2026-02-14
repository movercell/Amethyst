#pragma once

#define INSTANCE_MAX_COUNT 4096

#include "STDGLCamera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/geometry/Model.h"
#include "GLFW/glfw3.h"
#include <cstdint>
#include <memory>
#include <queue>

class STDGLMesh {
public:
    struct MeshInfo_t {
        float Radius;
    };
    GLuint VAO;
    GLuint VBO, EBO, MeshInfo;
    unsigned int IndexCount;

    STDGLMesh(const Geometry::Mesh& mesh);
};

class STDGLModel {
public:
    void Draw();
    void DrawDepth();

    STDGLModel(std::string path);
    ~STDGLModel();

    std::vector<STDGLMesh> Meshes;
};

class STDGLModelInstanceArray {
public:
    GLFWwindow* rendererData;
    uint32_t count = 0;
    GLuint InstanceBuffer = 0;
    std::queue<uint16_t> FreedIndeces;
    uint16_t NextIndex = 0;
    uint16_t InstanceMaxCount; // Used to limit the amount of static model instances to just what's needed.

    STDGLModelInstanceArray(GLFWwindow* data, uint16_t instancemaxcount = INSTANCE_MAX_COUNT);

    ~STDGLModelInstanceArray();
        
    std::unique_ptr<ModelInstance> MakeModelInstance();
    void Draw();

    friend class GLModelInstance;
};

class STDGLModelInstance : public ModelInstance {
public:
    STDGLModelInstanceArray* parent = nullptr;
    uint16_t index;

    void SetMatrix(mat4 Matrix);

    ~STDGLModelInstance();
    
    friend class GLModelInstanceArray;

    STDGLModelInstance(uint16_t Index, STDGLModelInstanceArray* Parent) { index = Index; parent = Parent; }
};