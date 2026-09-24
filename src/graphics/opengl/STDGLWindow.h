#pragma once

#include "engine/Resource.h"
#include "engine/graphics/Window.h"
#include <GLFW/glfw3.h>
#include "imgui.h"

struct STDGLWindow : public Window {
    Engine::Reference<Renderer> rendererRef;
    GLFWwindow* rendererData = nullptr;
    ImGuiContext* UIData = nullptr;
    GLFWwindow* data = nullptr;
    bool ShouldEatCursor = false;
    bool Fullscreen = false;
    int Width;
    int Height;

    bool NeedsUpdate = true;
    
    void ProcessCursorEating();
    void Update();

    ~STDGLWindow();
    STDGLWindow(Engine::Reference<Renderer> Renderer, GLFWwindow* RendererDataPtr, int ResX, int ResY, std::string name);

    void SetEatCursor(bool state);
    bool IsEatingCursor();
    bool IsWindowInFocus();
    void Draw();
    void SetName(std::string name);
    void SetResolution(int x, int y);
    int GetWidth();
    int GetHeight();
    void SetFullscreen(bool state);
};