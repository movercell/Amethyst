#include "graphics/Window.h"
#include "graphics/Renderer.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <iostream>

void Window::Update() {
    if (data != nullptr) {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(data));
        ImGui::DestroyContext(reinterpret_cast<ImGuiContext*>(UIData));
    }
    data = reinterpret_cast<____WindowData*>(glfwCreateWindow(800, 600, "LearnOpenGL", NULL, reinterpret_cast<GLFWwindow*>(rendererData)));
    UIData = rendererRef->UINewData(data);
    std::cout << UIData;
}

Window::~Window() {
    if (data != nullptr) {
        GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
        glfwDestroyWindow(temp);
    }
};