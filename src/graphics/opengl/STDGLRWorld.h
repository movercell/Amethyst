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

        friend class GLModelInstance;
    };

    class GLModelInstance : public ModelInstance {
    public:
        ModelInstanceArray* parent = nullptr;
        uint32_t index;

        void SetMatrix(mat4 Matrix);

        ~GLModelInstance() {

        }
        friend class GLModelInstanceArray;
    };


    ~STDGLRWorld();
    STDGLRWorld(std::weak_ptr<Renderer> Renderer) {
        renderer = std::static_pointer_cast<STDGLRenderer>(Renderer.lock());
    }

    std::vector<std::weak_ptr<STDGLCamera>> CameraVec;
    std::shared_ptr<STDGLRenderer> renderer;
};