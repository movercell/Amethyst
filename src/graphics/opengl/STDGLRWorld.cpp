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

std::unique_ptr<ModelInstance> STDGLRWorld::MakeModelInstance(std::string path) {
    glfwMakeContextCurrent(context);
    std::unique_ptr<ModelInstance> ret;

    try {
        ret = InstanceArrays.at(path).MakeModelInstance();
    } catch(std::out_of_range) {
        auto array = InstanceArrays.emplace(path, context).first;
        ret = array->second.MakeModelInstance();
    }

    return ret;
}

STDGLRWorld::~STDGLRWorld() {

}