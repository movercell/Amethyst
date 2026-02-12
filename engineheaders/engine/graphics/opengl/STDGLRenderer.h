#pragma once
#include "engine/graphics/RWorld.h"
#include "engine/master.h"

#include "engine/graphics/Window.h"
#include "engine/graphics/Renderer.h"
#include <vector>
#include <cstdint>
#include "engine/weak_vector.h"

/*!
*   \brief An OpenGL renderer.
*/
class ENGINEEXPORT STDGLRenderer : public Renderer
{
#ifdef AMETHYSTENGINESRC
private:
    ____UIData* UINewData(____WindowData* window);
    void UIDeleteData(____UIData* data);
    void UINewFrame();
    void UIEndFrame();

    friend class STDGLRWorld;
    friend class STDGLCamera;

    weak_vector<RWorld> RWorldVec;

    uint32_t CameraUBO;
#endif
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    std::shared_ptr<RWorld> MakeRWorld();
    Camera* GetCamera(std::string name);
    void Draw();
};
