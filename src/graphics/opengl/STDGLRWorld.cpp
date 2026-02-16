#include "STDGLRWorld.h"
#include "GLFW/glfw3.h"
#include "STDGLCamera.h"
#include "engine/graphics/Camera.h"
#include "engine/master.h"
#include <memory>


std::shared_ptr<Camera> STDGLRWorld::MakeCamera(vec2 resolution, const std::string& name, vec3 position, float yaw, float pitch) {
    glfwMakeContextCurrent(context);
    std::shared_ptr<STDGLCamera> result = std::make_shared<STDGLCamera>(context, resolution, name, position, yaw, pitch);
    CameraVec.push_back(result);

    return result;
}

Camera* STDGLRWorld::GetCamera(const std::string& name) {
    auto SharedCameraVec = CameraVec.lock();
    for (std::shared_ptr<Camera> camera : SharedCameraVec) {
        if (!camera) continue;
        if (camera->Name == name)
            return camera.get();
    }
    return nullptr;
}

std::unique_ptr<ModelInstance> STDGLRWorld::MakeModelInstance(const std::string& path) {
    glfwMakeContextCurrent(context);

    auto InstanceArraysShared = InstanceArrays.lock();
    for (auto& array : InstanceArraysShared) {
        if (array->Model->Path == path) return array->MakeModelInstance();
    }

    auto array = std::make_shared<STDGLModelInstanceArray>(context, modelsystem->GetModel(path));
    array->selfRef = array;
    InstanceArrays.push_back(array);
    return array->MakeModelInstance();
}

STDGLRWorld::~STDGLRWorld() {

}