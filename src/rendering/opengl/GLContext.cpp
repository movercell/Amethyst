#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <memory>
#include "rendering/Window.h"
#include "rendering/opengl/GLContext.h"

std::shared_ptr<Context> GLContext::Make() {

    std::shared_ptr<GLContext> tempContextRef = std::make_shared<GLContext>();

    tempContextRef->selfRef = std::static_pointer_cast<Context>(tempContextRef);


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    tempContextRef->contextData = reinterpret_cast<____WindowData*>(glfwCreateWindow(1, 1, "The “onosecond” is the second after you make a terrible mistake. The second when you realise what you just did", NULL, NULL));
    glfwDefaultWindowHints();
    return tempContextRef;
}

GLContext::~GLContext() {
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(contextData));
}

void GLContext::Draw() {
    for (std::weak_ptr<____Window> window : windowVector) {
        //TODO: make a Dear ImGUI context for each window
        glViewport(0, 0, 800, 600);
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
        window.lock()->Draw();
        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
}
