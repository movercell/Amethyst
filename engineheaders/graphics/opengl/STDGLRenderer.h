#pragma once
#include "master.h"

#include "graphics/Window.h"
#include "graphics/Renderer.h"
#include <vector>
#include <cstdint>
#include "../../src/model.h"
#include "../../src/shader.h"
/*!
*   \brief An OpenGL renderer.
*/
class ENGINEEXPORT STDGLRenderer : public Renderer
{
private:
    ____UIData* UINewData(____WindowData* window);
    void UINewFrame();
    void UIEndFrame();

    std::vector<void*> RWorldVec;
    uint32_t CameraUBO;
    Shader shader = Shader("scripts/shaders/opengl/generic.vs", "scripts/shaders/opengl/generic.fs");
    Model model = Model("Untitled2.glb");
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    RWorld* newRWorld();
    void deleteRWorld(RWorld* target);
    void Draw();
};
