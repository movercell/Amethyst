#include "STDGLRWorld.h"
#include "GLFW/glfw3.h"
#include "STDGLCamera.h"
#include <memory>

std::shared_ptr<Camera> STDGLRWorld::MakeCamera(vec2 resolution, const std::string& name, vec3 position, float yaw, float pitch) {
    auto temp = glfwGetCurrentContext();
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(renderer->rendererData));
    std::shared_ptr<STDGLCamera> result = std::make_shared<STDGLCamera>(resolution, name, position, yaw, pitch);
    CameraVec.push_back(result);
    glfwMakeContextCurrent(temp);

    return result;
}

STDGLRWorld::~STDGLRWorld() {

}