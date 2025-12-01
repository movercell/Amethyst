#pragma once

#define INSTANCE_MAX_COUNT 4096

#include "STDGLCamera.h"
#include "graphics/ModelInstance.h"
#include "graphics/RWorld.h"
#include "graphics/opengl/STDGLRenderer.h"
#include "GLFW/glfw3.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <queue>

class STDGLRWorld : public RWorld {
public:
    std::shared_ptr<Camera> MakeCamera(vec2 resolution, const std::string& name, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH);
    virtual Camera* GetCamera(std::string name);
    std::shared_ptr<ModelInstance> MakeModelInstance();

    class ModelInstanceArray {
    public:
        GLFWwindow* rendererData;
        uint32_t count = 0;
        GLuint InstanceBuffer = 0;
        std::queue<uint16_t> FreedIndeces;
        uint16_t NextIndex = 0;
        uint16_t InstanceMaxCount;

        void init(GLFWwindow* data, uint16_t instancemaxcount = INSTANCE_MAX_COUNT) {
            rendererData = data;
            InstanceMaxCount = instancemaxcount;
            auto temp = glfwGetCurrentContext();
            glfwMakeContextCurrent(rendererData);

            glCreateBuffers(1, &InstanceBuffer);
            uint8_t* temparr = new uint8_t[InstanceMaxCount * sizeof(mat4)]; // To init buffer to all NaN
            for (int i = 0; i < InstanceMaxCount * sizeof(mat4); i++) {
                temparr[i] = 0xFF;
            }
            glNamedBufferData(InstanceBuffer, InstanceMaxCount * sizeof(mat4), temparr, GL_DYNAMIC_DRAW);

            glfwMakeContextCurrent(temp);
        }
        
        std::shared_ptr<ModelInstance> MakeModelInstance();

        friend class GLModelInstance;
    };

    class GLModelInstance : public ModelInstance {
    public:
        ModelInstanceArray* parent = nullptr;
        uint16_t index;

        void SetMatrix(mat4 Matrix);

        ~GLModelInstance();
    
        friend class GLModelInstanceArray;

        GLModelInstance(uint16_t Index, ModelInstanceArray* Parent) { index = Index; parent = Parent; }
    };
    ModelInstanceArray tmpinstancearr;


    ~STDGLRWorld();
    STDGLRWorld(std::weak_ptr<Renderer> Renderer) {
        renderer = std::static_pointer_cast<STDGLRenderer>(Renderer.lock());
        tmpinstancearr.init(reinterpret_cast<GLFWwindow*>(renderer->rendererData));
    }

    std::vector<std::weak_ptr<STDGLCamera>> CameraVec;
    std::shared_ptr<STDGLRenderer> renderer;
};