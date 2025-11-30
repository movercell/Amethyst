#include "STDGLRWorld.h"
#include "GLFW/glfw3.h"
#include "STDGLCamera.h"
#include "graphics/Camera.h"
#include <memory>


void STDGLRWorld::GLModelInstance::SetMatrix(mat4 Matrix) {
    auto temp = glfwGetCurrentContext();
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), &Matrix);

    glfwMakeContextCurrent(temp);
}



std::shared_ptr<Camera> STDGLRWorld::MakeCamera(vec2 resolution, const std::string& name, vec3 position, float yaw, float pitch) {
    auto temp = glfwGetCurrentContext();
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(renderer->rendererData));
    std::shared_ptr<STDGLCamera> result = std::make_shared<STDGLCamera>(resolution, name, position, yaw, pitch);
    CameraVec.push_back(result);
    glfwMakeContextCurrent(temp);

    return result;
}

Camera* STDGLRWorld::GetCamera(std::string name) {
    for (int CameraI = 0; CameraI < CameraVec.size(); CameraI++) {
        Camera* result = CameraVec[CameraI].lock().get();

        // Check whether the camera still exists.
        if (!result) {
            CameraI--;
            CameraVec.erase(CameraVec.begin() + CameraI);
            CameraVec.pop_back();
            continue;
        }
        if (result->Name == name)
            return result;
    }
    return nullptr;
}

std::shared_ptr<ModelInstance> STDGLRWorld::MakeModelInstance() {

}

STDGLRWorld::~STDGLRWorld() {

}