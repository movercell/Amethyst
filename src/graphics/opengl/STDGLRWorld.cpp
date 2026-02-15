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

    auto array = InstanceArrays.try_emplace(path, context, modelsystem->GetModel(path)).first;
    return array->second.MakeModelInstance();
}

STDGLRWorld::~STDGLRWorld() {

}