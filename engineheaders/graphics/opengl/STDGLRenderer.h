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
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    void Draw();
};
