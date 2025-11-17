#include "graphics/Renderer.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

//first the internal window stuff

Renderer::____Window::____Window(std::weak_ptr<Renderer> RendererWeakPtr, ____WindowData* RendererDataPtr) {
    rendererRef = RendererWeakPtr.lock();
}

void Renderer::____Window::Draw() {

    GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
    if (temp == nullptr) {
        return; //TODO: make it crash when there's no actual window
    }
    glfwMakeContextCurrent(temp);
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(UIData));
    rendererRef->UINewFrame();

    //TODO: run the UI function HERE
    static bool isWindowOpen;
    ImGui::Begin("hi", &isWindowOpen);
    ImGui::End();

    rendererRef->UIEndFrame();

    glfwSwapBuffers(temp);
}

//now the context stuff

std::shared_ptr<Window> Renderer::MakeWindow() {
    std::shared_ptr<____Window> tempRef = std::make_shared<____Window>(selfRef, rendererData);
    windowVector.push_back(tempRef);
    return std::static_pointer_cast<Window>(tempRef);
}