#include <glad/glad.h>
#include "STDGLWindow.h"
#include "GLFW/glfw3.h"
#include "engine/filesystem/Filesystem.h"
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
    NeedsUpdate = true;
}
void STDGLWindow::SetResolution(int x, int y) {
    Width = x;
    Height = y;
    NeedsUpdate = true;
}
void STDGLWindow::SetFullscreen(bool state) {
    if (Fullscreen == state) return;
    Fullscreen = state;
    NeedsUpdate = true;
}

int STDGLWindow::GetWidth() {
   return Width;
}
int STDGLWindow::GetHeight() {
   return Height;
}

void STDGLWindow::Update() {
    if (data != nullptr) {
        ImGui::SetCurrentContext(UIData);

        ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplGlfw_Shutdown();
	    ImGui::DestroyContext();

		glfwDestroyWindow(data);
    }

    GLFWmonitor* primarymonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videomode = glfwGetVideoMode(primarymonitor);
    GLFWmonitor* monitor = nullptr;

    glfwDefaultWindowHints();

	glfwWindowHint(GLFW_SAMPLES, 16);
#ifndef DEBUG
    // TODO: Should add a setting to make it override NO_ERROR even in release
    glfwWindowHint(GLFW_NO_ERROR, GLFW_TRUE);
#endif
    if (Fullscreen) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        glfwWindowHint(GLFW_RED_BITS, videomode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, videomode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, videomode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, videomode->refreshRate);
        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
        Width = videomode->width;
        Height = videomode->height;
        monitor = primarymonitor;
    }
    data = glfwCreateWindow(Width, Height, Name.c_str(), monitor, reinterpret_cast<GLFWwindow*>(rendererData));
    glfwSwapInterval(1); // TODO: add a vsync setting

    if (Fullscreen) {
        int xpos, ypos;
        glfwGetMonitorPos(primarymonitor, &xpos, &ypos);
        glfwSetWindowPos(data, xpos, ypos);
    }

    ProcessCursorEating();

    {
	    IMGUI_CHECKVERSION();
	    UIData = ImGui::CreateContext(rendererRef->GetFontAtlas());
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
    if (NeedsUpdate) {
        Update();
        NeedsUpdate = false;
    }

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
