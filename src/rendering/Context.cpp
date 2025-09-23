#include "rendering/Context.h"
#include <GLFW/glfw3.h>

//first the internal window stuff

void Context::____Window::Update() {
    data = reinterpret_cast<____WindowData*>(glfwCreateWindow(800, 600, "LearnOpenGL", NULL, reinterpret_cast<GLFWwindow*>(contextData)));
}

Context::____Window::____Window(std::weak_ptr<Context> ContextWeakPtr, ____WindowData* ContextDataPtr) {
    contextRef = ContextWeakPtr.lock();
}
void Context::____Window::Draw() {

}