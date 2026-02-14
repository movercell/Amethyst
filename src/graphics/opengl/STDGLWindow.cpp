#include <glad/glad.h>
#include "STDGLWindow.h"
#include "GLFW/glfw3.h"
#include "engine/graphics/Renderer.h"
#include "GLMisc.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

void STDGLWindow::EatCursor(bool state) {
    ShouldEatCursor = state;
    ProcessCursorEating();
}

void STDGLWindow::ProcessCursorEating() {
    if (ShouldEatCursor) {
        glfwSetInputMode(data, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(data, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool STDGLWindow::IsEatingCursor() {
    return ShouldEatCursor;
}

bool STDGLWindow::IsWindowInFocus() {
    return glfwGetWindowAttrib(data, GLFW_FOCUSED);
}

void STDGLWindow::Update() {
    if (data != nullptr) {
        glfwDestroyWindow(data);
        
        ImGui::SetCurrentContext(UIData);

        ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplGlfw_Shutdown();
	    ImGui::DestroyContext();
    }
	glfwWindowHint(GLFW_SAMPLES, 16);
    data = glfwCreateWindow(Resolution.x, Resolution.y, Name.c_str(), NULL, reinterpret_cast<GLFWwindow*>(rendererData));

    ProcessCursorEating();

    {
	    IMGUI_CHECKVERSION();
	    UIData = ImGui::CreateContext();
        ImGui::SetCurrentContext(UIData);
	    ImGuiIO& io = ImGui::GetIO();
	    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

        ImGui_ImplGlfw_InitForOpenGL(data, false);
        ImGui_ImplOpenGL3_Init();

        GLMisc::windowSetCallbacks(data);
    }
    glfwSetWindowUserPointer(data, this);
}

STDGLWindow::~STDGLWindow() {
    if (data != nullptr) {
        glfwDestroyWindow(data);
    }
};

STDGLWindow::STDGLWindow(std::weak_ptr<Renderer> RendererWeakPtr, GLFWwindow* RendererDataPtr) {
    rendererData = RendererDataPtr;
    rendererRef = RendererWeakPtr.lock();
}

void STDGLWindow::Draw() {
    glfwMakeContextCurrent(data);
    glViewport(0, 0, Resolution.x, Resolution.y);
    
    ImGui::SetCurrentContext(UIData);
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    if (UIFunction) {
        UIFunction(rendererRef.get(), this);
    } else {
        exit(-2);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(data);
}