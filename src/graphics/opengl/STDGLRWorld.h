#pragma once

#include "STDGLCamera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/RWorld.h"
#include "engine/Resource.h"
#include "STDGLRenderer.h"
#include <memory>
#include <vector>
#include "STDGLModel.h"
#include "STDGLLight.h"

struct STDGLRWorld : public RWorld {
    Engine::Reference<Camera> MakeCamera(vec2 resolution, const std::string& name, float FOV = CAMERA_DEFAULT_FOV, float Near = CAMERA_DEFAULT_NEAR, float Far = CAMERA_DEFAULT_FAR);
    Engine::Reference<Light> MakeSpotLight(vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far);
    virtual Camera* GetCamera(const std::string& name);
    std::unique_ptr<ModelInstance> MakeModelInstance(const std::string& path = "error.glb");

    ~STDGLRWorld();
    STDGLRWorld(Engine::Reference<Renderer> Renderer, STDGLModelSystem* ModelSystem) {
        context = glfwGetCurrentContext();
        renderer = Renderer;
        modelsystem = ModelSystem;
    }


    std::map<std::string, Engine::ManagedResource<STDGLRWorld, STDGLModelInstanceArray>*> InstanceArrays;
    std::vector<Engine::ManagedInterfacedResource<STDGLRWorld, Camera, STDGLCamera>*> CameraVec;
    Engine::Reference<Renderer> renderer;
    Engine::Resource<RWorld>* selfResource;
    GLFWwindow* context;
    STDGLModelSystem* modelsystem;
    STDGLLightSystem lightsystem;

    void _unmanage_resource(Engine::Resource<Camera>* res) {
        CameraVec.erase(std::find(CameraVec.begin(), CameraVec.end(), res));
        delete res;
    }
    void _unmanage_resource(Engine::Resource<STDGLModelInstanceArray>* res) {
        for (auto it = InstanceArrays.begin(); it != InstanceArrays.end(); ++it) {
            if (it->second == res) {
                InstanceArrays.erase(it);
                break;
            }
        }
        delete res;
    }
};