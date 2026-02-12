#include "engine/graphics/Window.h"
#include "GLFW/glfw3.h"
#include "engine/graphics/Renderer.h" // IWYU pragma: keep
#include "misc.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

void Window::EatCursor(bool state) {
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(data);
    ShouldEatCursor = state;
    ProcessCursorEating();
}

void Window::ProcessCursorEating() {
    auto* window = reinterpret_cast<GLFWwindow*>(data);
    if (ShouldEatCursor) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool Window::IsEatingCursor() {
    return ShouldEatCursor;
}

void Window::SetUIFunction(std::function<void(Renderer*, Window*)> Function) {
    UIFunction = Function;
}

bool Window::IsWindowInFocus() {
    return glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(data), GLFW_FOCUSED);
}

void Window::Update() {
    if (data != nullptr) {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(data));
        rendererRef->UIDeleteData(UIData);
    }
	glfwWindowHint(GLFW_SAMPLES, 16);
    data = reinterpret_cast<____WindowData*>(glfwCreateWindow(Resolution.x, Resolution.y, Name.c_str(), NULL, reinterpret_cast<GLFWwindow*>(rendererData)));

    ProcessCursorEating();

    UIData = rendererRef->UINewData(data);
    glfwSetWindowUserPointer(reinterpret_cast<GLFWwindow*>(data), reinterpret_cast<void*>(this));
}

Window::~Window() {
    if (data != nullptr) {
        GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
        glfwDestroyWindow(temp);
    }
};