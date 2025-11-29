#include "graphics/Window.h"
#include "GLFW/glfw3.h"
#include "graphics/Renderer.h" // IWYU pragma: keep
#include "misc.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

void Window::SetUIFunction(std::function<void(Renderer*, Window*)> Function) {
    UIFunction = Function;
}

void Window::EatCursor(bool state) {
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(data);
    if (state) {
        glfwSetCursorPos(window, 0, 0);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
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