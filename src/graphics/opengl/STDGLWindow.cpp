#include "STDGLWindow.h"
#include "GLFW/glfw3.h"
#include "engine/graphics/Renderer.h" // IWYU pragma: keep
#include "../misc.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

void STDGLWindow::EatCursor(bool state) {
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(data);
    ShouldEatCursor = state;
    ProcessCursorEating();
}

void STDGLWindow::ProcessCursorEating() {
    auto* window = reinterpret_cast<GLFWwindow*>(data);
    if (ShouldEatCursor) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool STDGLWindow::IsEatingCursor() {
    return ShouldEatCursor;
}

bool STDGLWindow::IsWindowInFocus() {
    return glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(data), GLFW_FOCUSED);
}

void STDGLWindow::Update() {
    if (data != nullptr) {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(data));
        
        ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(UIData));

        ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplGlfw_Shutdown();
	    ImGui::DestroyContext();
    }
	glfwWindowHint(GLFW_SAMPLES, 16);
    data = reinterpret_cast<____WindowData*>(glfwCreateWindow(Resolution.x, Resolution.y, Name.c_str(), NULL, reinterpret_cast<GLFWwindow*>(rendererData)));

    ProcessCursorEating();

    {
	    IMGUI_CHECKVERSION();
	    UIData = reinterpret_cast<____UIData*>(ImGui::CreateContext());
        ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(UIData));
	    ImGuiIO& io = ImGui::GetIO();
	    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

        ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(data), false);
        ImGui_ImplOpenGL3_Init();

        GLMisc::windowSetCallbacks(reinterpret_cast<GLFWwindow*>(data));
    }
    glfwSetWindowUserPointer(reinterpret_cast<GLFWwindow*>(data), reinterpret_cast<void*>(this));
}

STDGLWindow::~STDGLWindow() {
    if (data != nullptr) {
        GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
        glfwDestroyWindow(temp);
    }
};

STDGLWindow::STDGLWindow(std::weak_ptr<Renderer> RendererWeakPtr, ____WindowData* RendererDataPtr) {
    rendererData = RendererDataPtr;
    rendererRef = RendererWeakPtr.lock();
}

void STDGLWindow::Draw() {

    GLFWwindow* temp = reinterpret_cast<GLFWwindow*>(data);
    if (temp == nullptr) {
        return; //TODO: make it crash when there's no actual window
    }
    glfwMakeContextCurrent(temp);
    
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(UIData));
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

    glfwSwapBuffers(temp);
}