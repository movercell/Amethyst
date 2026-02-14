#pragma once
#include "engine/graphics/RWorld.h"
#include "engine/master.h"

#include "engine/graphics/Window.h"
#include "engine/graphics/Renderer.h"
#include <vector>
#include <cstdint>
#include "engine/weak_vector.h"
#ifdef AMETHYSTENGINESRC
    #include "../src/graphics/opengl/STDGLWindow.h"
#endif

/*!
*   \brief An OpenGL renderer.
*/
class ENGINEEXPORT STDGLRenderer : public Renderer
{
#ifdef AMETHYSTENGINESRC
private:
    ____WindowData* rendererData = nullptr;
    weak_vector<RWorld> RWorldVec;
    weak_vector<STDGLWindow> WindowVector = weak_vector<STDGLWindow>();
#endif
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    std::shared_ptr<RWorld> MakeRWorld();
    Camera* GetCamera(std::string name);
    void Draw();
    std::shared_ptr<Window> MakeWindow();
};
