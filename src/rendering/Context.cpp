#include "rendering/Context.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

//first the internal window stuff

Context::____Window::____Window(std::weak_ptr<Context> ContextWeakPtr, ____WindowData* ContextDataPtr) {
    contextRef = ContextWeakPtr.lock();
}

void Context::____Window::Draw() {

    GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
    if (temp == nullptr) {
        return; //TODO: make it crash when there's no actual window
    }
    glfwMakeContextCurrent(temp);

    //TODO: run the UI function HERE
    static bool isWindowOpen;
    ImGui::Begin("hi", &isWindowOpen);
    ImGui::End();

    glfwSwapBuffers(temp);
}

//now the context stuff

std::shared_ptr<Window> Context::MakeWindow() {
    std::shared_ptr<____Window> tempRef = std::make_shared<____Window>(selfRef, contextData);
    windowVector.push_back(tempRef);
    return std::static_pointer_cast<Window>(tempRef);
}