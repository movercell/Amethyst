#pragma once

#include "engine/graphics/Window.h"

class STDGLWindow : public Window {
protected:
    std::shared_ptr<Renderer> rendererRef = nullptr;
    ____WindowData* rendererData = nullptr;
    ____UIData* UIData = nullptr;
    ____WindowData* data = nullptr;
    bool ShouldEatCursor = false;
    void ProcessCursorEating();

    friend class GLMisc;
public:
    ~STDGLWindow();
    STDGLWindow(std::weak_ptr<Renderer> RendererWeakPtr, ____WindowData* RendererDataPtr);

    void Update();
    void EatCursor(bool state);
    bool IsEatingCursor();
    bool IsWindowInFocus();
    void Draw();
};