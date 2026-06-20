#include "STDGLRWorld.h"
#include "GLFW/glfw3.h"
#include "STDGLCamera.h"
#include "engine/graphics/Camera.h"
#include "engine/master.h"
#include <memory>


Engine::Reference<Camera> STDGLRWorld::MakeCamera(vec2 resolution, const std::string& name, vec3 position, float yaw, float pitch) {
    glfwMakeContextCurrent(context);
    auto result = new Engine::ManagedInterfacedResource<STDGLRWorld, Camera, STDGLCamera>(this, context, &(renderer->GetFrameCounter()), resolution, name, position, yaw, pitch);
    CameraVec.push_back(result);

    return result;
}

Camera* STDGLRWorld::GetCamera(const std::string& name) {
    for (Engine::Resource<Camera>* camerares : CameraVec) {
        auto camera = camerares->Get();
        if (camera->Name == name)
            return camera;
    }
    return nullptr;
}

std::unique_ptr<ModelInstance> STDGLRWorld::MakeModelInstance(const std::string& path) {
    try {
        return InstanceArrays.at(path)->resource.MakeModelInstance();
    } catch(...) {
        glfwMakeContextCurrent(context);
        auto array = new Engine::ManagedResource<STDGLRWorld, STDGLModelInstanceArray>(this, context, modelsystem->GetModel(path));
        array->resource.selfResource = array;
        InstanceArrays.emplace(path, array);
        return array->resource.MakeModelInstance();
    }
}

STDGLRWorld::~STDGLRWorld() {

}