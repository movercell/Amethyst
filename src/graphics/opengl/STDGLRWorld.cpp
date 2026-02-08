#include "STDGLRWorld.h"
#include "GLFW/glfw3.h"
#include "STDGLCamera.h"
#include "graphics/Camera.h"
#include "master.h"
#include <memory>


std::shared_ptr<Camera> STDGLRWorld::MakeCamera(vec2 resolution, const std::string& name, vec3 position, float yaw, float pitch) {
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(renderer->rendererData));
    std::shared_ptr<STDGLCamera> result = std::make_shared<STDGLCamera>(renderer, resolution, name, position, yaw, pitch);
    CameraVec.push_back(result);

    return result;
}

Camera* STDGLRWorld::GetCamera(std::string name) {
    auto SharedCameraVec = CameraVec.lock();
    for (std::shared_ptr<Camera> camera : SharedCameraVec) {
        if (!camera) continue;
        if (camera->Name == name)
            return camera.get();
    }
    return nullptr;
}

std::shared_ptr<ModelInstance> STDGLRWorld::MakeModelInstance() {
    return tmpinstancearr.MakeModelInstance();
}

STDGLRWorld::~STDGLRWorld() {

}