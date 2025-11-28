#include "graphics/Window.h"
#include "graphics/Renderer.h" // IWYU pragma: keep
#include "misc.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <iostream>

void Window::SetUIFunction(std::function<void()> Function) {
    UIFunction = Function;
}

void Window::Update() {
    if (data != nullptr) {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(data));
        ImGui::DestroyContext(reinterpret_cast<ImGuiContext*>(UIData));
    }
    data = reinterpret_cast<____WindowData*>(glfwCreateWindow(800, 600, "LearnOpenGL", NULL, reinterpret_cast<GLFWwindow*>(rendererData)));
    UIData = rendererRef->UINewData(data);
}

Window::~Window() {
    if (data != nullptr) {
        GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
        glfwDestroyWindow(temp);
    }
};