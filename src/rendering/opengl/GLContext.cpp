#include <GLFW/glfw3.h>
#include <memory>
#include "rendering/Window.h"
#include "GLContext.h"

std::shared_ptr<Context> GLContext::Make() {

    std::shared_ptr<GLContext> tempContextRef = std::make_shared<GLContext>();

    tempContextRef->selfRef = std::static_pointer_cast<Context>(tempContextRef);


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    tempContextRef->contextData = reinterpret_cast<____WindowData*>(glfwCreateWindow(1, 1, "The “onosecond” is the second after you make a terrible mistake. The second when you realise what you just did", NULL, NULL));
    return tempContextRef;
}

GLContext::~GLContext() {

}

void GLContext::Render() {

}