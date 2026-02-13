#pragma once

#define INSTANCE_MAX_COUNT 4096

#include "STDGLCamera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/opengl/STDGLRenderer.h"
#include <memory>
#include <vector>
#include "STDGLModel.h"

class STDGLRWorld : public RWorld {
public:
    std::shared_ptr<Camera> MakeCamera(vec2 resolution, const std::string& name, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH);
    virtual Camera* GetCamera(std::string name);
    std::shared_ptr<ModelInstance> MakeModelInstance();

    STDGLModelInstanceArray tmpinstancearr;


    ~STDGLRWorld();
    STDGLRWorld(std::weak_ptr<Renderer> Renderer, GLFWwindow* Context) {
        renderer = Renderer.lock();
        context = Context;
        tmpinstancearr.init(context);
    }

    weak_vector<STDGLCamera> CameraVec;
    std::shared_ptr<Renderer> renderer;
    GLFWwindow* context;
};