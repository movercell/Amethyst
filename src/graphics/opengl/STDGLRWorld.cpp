#include "STDGLRWorld.h"
#include "GLFW/glfw3.h"
#include "STDGLCamera.h"
#include "engine/graphics/Camera.h"
#include "engine/master.h"
#include <memory>


Engine::Reference<Camera> STDGLRWorld::MakeCamera(vec2 resolution, const std::string& name, float FOV, float Near, float Far) {
    glfwMakeContextCurrent(context);
    auto result = new Engine::ManagedInterfacedResource<STDGLRWorld, Camera, STDGLCamera>(this, selfResource, resolution, name, FOV, Near, Far);
    CameraVec.push_back(result);

    return result;
}
Engine::Reference<Light> STDGLRWorld::MakeSpotLight(vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far) {
    glfwMakeContextCurrent(context);
    return lightsystem.MakeLight(selfResource, STDGLLightType::Spot, resolution, inner_cutoff_angle, outer_cutoff_angle, color, near, far);
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