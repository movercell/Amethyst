#pragma once
#include "master.h"

#include "graphics/Window.h"
#include "graphics/Renderer.h"

/*!
*   \brief An OpenGL renderer.
*/
class ENGINEEXPORT STDGLRenderer : public Renderer
{
private:
    /* data */
    ____UIData* UINewData(____WindowData* window);
    void UINewFrame();
    void UIEndFrame();
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    void Draw();
};
