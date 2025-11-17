#include "graphics/Window.h"
#include <GLFW/glfw3.h>

void Window::Update() {
    if (data != nullptr) {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(data));
    }
    data = reinterpret_cast<____WindowData*>(glfwCreateWindow(800, 600, "LearnOpenGL", NULL, reinterpret_cast<GLFWwindow*>(rendererData)));
}

Window::~Window() {
    if (data != nullptr) {
        GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
        glfwDestroyWindow(temp);
    }
};