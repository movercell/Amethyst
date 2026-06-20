#include <glad/glad.h>
#include "STDGLWindow.h"
#include "GLFW/glfw3.h"
#include "engine/graphics/Renderer.h"
#include "GLMisc.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

void STDGLWindow::SetEatCursor(bool state) {
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


void STDGLWindow::SetName(std::string name) {
    Name = name;
    Update();
}
void STDGLWindow::SetResolution(int x, int y) {
    Width = x;
    Height = y;
    Update();
}

void STDGLWindow::Update() {
    if (data != nullptr) {
        ImGui::SetCurrentContext(UIData);

        ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplGlfw_Shutdown();
	    ImGui::DestroyContext();

		glfwDestroyWindow(data);
    }

    glfwDefaultWindowHints();

	glfwWindowHint(GLFW_SAMPLES, 16);
#ifndef DEBUG
    // TODO: Should add a setting to make it override NO_ERROR even in release
    glfwWindowHint(GLFW_NO_ERROR, GLFW_TRUE);
#endif

    data = glfwCreateWindow(Width, Height, Name.c_str(), NULL, reinterpret_cast<GLFWwindow*>(rendererData));
    glfwSwapInterval(1); // TODO: add a vsync setting

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
        ImGui::SetCurrentContext(UIData);

        ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplGlfw_Shutdown();
	    ImGui::DestroyContext();
		
		glfwDestroyWindow(data);
    }
};

STDGLWindow::STDGLWindow(Engine::Reference<Renderer> Renderer, GLFWwindow* RendererDataPtr, int ResX, int ResY, std::string name) {
    rendererData = RendererDataPtr;
    rendererRef = Renderer;
    Width = ResX;
    Height = ResY;
    Name = name;
    Update();
}

void STDGLWindow::Draw() {
    glfwMakeContextCurrent(data);
    glViewport(0, 0, Width, Height);
    
    ImGui::SetCurrentContext(UIData);
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    if (UIFunction) {
        UIFunction(rendererRef.get(), this);
    } else {
        Engine::Error("A window was drawn without a UI function!");
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(data);
}
