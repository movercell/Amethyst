#pragma once

#include "engine/graphics/Window.h"
#include <GLFW/glfw3.h>
#include "imgui.h"

class STDGLWindow : public Window {
protected:
    std::shared_ptr<Renderer> rendererRef = nullptr;
    GLFWwindow* rendererData = nullptr;
    ImGuiContext* UIData = nullptr;
    GLFWwindow* data = nullptr;
    bool ShouldEatCursor = false;
    void ProcessCursorEating();

    friend class GLMisc;
public:
    ~STDGLWindow();
    STDGLWindow(std::weak_ptr<Renderer> RendererWeakPtr, GLFWwindow* RendererDataPtr);

    void Update();
    void EatCursor(bool state);
    bool IsEatingCursor();
    bool IsWindowInFocus();
    void Draw();
};