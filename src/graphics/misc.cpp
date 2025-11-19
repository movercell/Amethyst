#include <imgui.h>
#include "misc.h"

void GraphicsMisc::windowFocusCallback(GLFWwindow* window, int focused) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_WindowFocusCallback(window, focused);

    ImGui::SetCurrentContext(temp);
}

void GraphicsMisc::windowCursorEnterCallback(GLFWwindow* window, int entered) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_CursorEnterCallback(window, entered);

    ImGui::SetCurrentContext(temp);
}

void GraphicsMisc::windowCursorPosCallback(GLFWwindow* window, double x, double y) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_CursorPosCallback(window, x, y);

    ImGui::SetCurrentContext(temp);
}

void GraphicsMisc::windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    ImGui::SetCurrentContext(temp);
}

void GraphicsMisc::windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    ImGui::SetCurrentContext(temp);
}

void GraphicsMisc::windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    ImGui::SetCurrentContext(temp);
}

void GraphicsMisc::windowCharCallback(GLFWwindow* window, unsigned int c) {
    ImGuiContext* temp = ImGui::GetCurrentContext();
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(glfwGetWindowUserPointer(window)));

    ImGui_ImplGlfw_CharCallback(window, c);

    ImGui::SetCurrentContext(temp);
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