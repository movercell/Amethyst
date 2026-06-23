#pragma once
#include "engine/graphics/RWorld.h"
#include "engine/master.h"

#include "engine/graphics/Window.h"
#include "engine/graphics/Renderer.h"
#include <vector>
#include <cstdint>
#include "engine/weak_vector.h"
#include "STDGLModel.h"
#include "STDGLWindow.h"
#include "STDGLShaderSystem.h"

/*!
*   \brief An OpenGL renderer.
*/
class STDGLRenderer : public Renderer {
protected:
    GLFWwindow* rendererData = nullptr;
    std::vector<Engine::Resource<RWorld>*> RWorldVec;
    std::vector<Engine::ManagedInterfacedResource<STDGLRenderer, Window, STDGLWindow>*> WindowVector;
    STDGLModelSystem ModelSystem;
    STDGLShaderSystem ShaderSystem;

    Engine::UnmanagedInterfacedResource<Renderer, STDGLRenderer>* selfResource; // Stored to be able to make the Engine::Reference objects for objects that it gives out.

    std::array<GLsync, 2> DoubleBufferFences = { nullptr, nullptr };

    GLuint ModelInstancePreprocessShader;
    GLuint ModelInstanceReplicatorShader;

    uint64_t FrameCounter = 0;

    void Init();

    template<typename Container, typename Interface, typename T>
    friend class Engine::ManagedInterfacedResource;
    void _unmanage_resource(Engine::Resource<Window>* res) {
        WindowVector.erase(std::find(WindowVector.begin(), WindowVector.end(), res));
        delete res;
    }
    void _unmanage_resource(Engine::Resource<RWorld>* res) {
        RWorldVec.erase(std::find(RWorldVec.begin(), RWorldVec.end(), res));
        delete res;
    }
    template<bool isDepth>
    void DrawIArrays(std::vector<Engine::Reference<STDGLModelInstanceArray>>& InstanceArrayRefs);
public:
    ~STDGLRenderer();
    static Engine::Reference<Renderer> Make();
    Engine::Reference<RWorld> MakeRWorld();
    Camera* GetCamera(std::string name);
    const uint64_t& GetFrameCounter();
    void Draw();
    Engine::Reference<Window> MakeWindow(int x, int y, std::string name);
};
