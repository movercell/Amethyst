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
    #include "../src/graphics/opengl/STDGLModel.h"
    #include "../src/graphics/opengl/STDGLShaderSystem.h"
#endif

/*!
*   \brief An OpenGL renderer.
*/
class ENGINEEXPORT STDGLRenderer : public Renderer {
#ifdef AMETHYSTENGINESRC
protected:
    GLFWwindow* rendererData = nullptr;
    weak_vector<RWorld> RWorldVec;
    weak_vector<STDGLWindow> WindowVector = weak_vector<STDGLWindow>();
    STDGLModelSystem ModelSystem;
    STDGLShaderSystem ShaderSystem;

    GLsync DoubleBufferFences[2] = { nullptr, nullptr };

    GLuint ModelIndirectReplicationShader;
    GLuint ModelInstancePreprocessShader;

    uint64_t FrameCounter = 0;
    
#endif
public:
    ~STDGLRenderer();
    //!Makes the renderer
    static std::shared_ptr<Renderer> Make();
    std::shared_ptr<RWorld> MakeRWorld();
    Camera* GetCamera(std::string name);
    const uint64_t& GetFrameCounter();
    void Draw();
    std::shared_ptr<Window> MakeWindow();
};
