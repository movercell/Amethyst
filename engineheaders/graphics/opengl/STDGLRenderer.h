#pragma once
#include "graphics/RWorld.h"
#include "master.h"

#include "graphics/Window.h"
#include "graphics/Renderer.h"
#include <vector>
#include <cstdint>
/*!
*   \brief An OpenGL renderer.
*/
class ENGINEEXPORT STDGLRenderer : public Renderer
{
private:
    ____UIData* UINewData(____WindowData* window);
    void UINewFrame();
    void UIEndFrame();

    friend class STDGLRWorld;

    std::vector<RWorld*> RWorldVec;
    uint32_t CameraUBO;
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    RWorld* newRWorld();
    void deleteRWorld(RWorld* target);
    Camera* GetCamera(std::string name);
    void Draw();
};
