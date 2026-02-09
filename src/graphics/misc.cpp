#include <imgui.h>
#include "misc.h"
#include "graphics/Window.h"

void GraphicsMisc::windowFocusCallback(GLFWwindow* window, int focused) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    if (focused = GLFW_TRUE) {
        WindowObject->ProcessCursorEating();
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    ImGui_ImplGlfw_WindowFocusCallback(window, focused);
}

void GraphicsMisc::windowCursorEnterCallback(GLFWwindow* window, int entered) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}

void GraphicsMisc::windowCursorPosCallback(GLFWwindow* window, double x, double y) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
}

void GraphicsMisc::windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void GraphicsMisc::windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void GraphicsMisc::windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void GraphicsMisc::windowCharCallback(GLFWwindow* window, unsigned int c) {
    auto* WindowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(WindowObject->UIData));

    ImGui_ImplGlfw_CharCallback(window, c);
}


void GraphicsMisc::windowSetCallbacks(GLFWwindow* window) {
    glfwSetWindowFocusCallback(window, windowFocusCallback);
    glfwSetCursorEnterCallback(window, windowCursorEnterCallback);
    glfwSetCursorPosCallback(window, windowCursorPosCallback);
    glfwSetMouseButtonCallback(window, windowMouseButtonCallback);
    glfwSetScrollCallback(window, windowScrollCallback);
    glfwSetKeyCallback(window, windowKeyCallback);
    glfwSetCharCallback(window, windowCharCallback);
}