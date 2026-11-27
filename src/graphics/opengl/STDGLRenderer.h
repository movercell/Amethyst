#pragma once
#include "engine/graphics/RWorld.h"
#include "engine/master.h"

#include "engine/graphics/Window.h"
#include "engine/graphics/Renderer.h"
#include <vector>
#include <cstdint>
#include "STDGLModel.h"
#include "STDGLWindow.h"
#include "STDGLMaterial.h"
#include "STDGLShaderSystem.h"
#include "GLMisc.h"

/*!
*   \brief An OpenGL renderer.
*/
class STDGLRenderer : public Renderer {
protected:
    GLContext Context;
    std::vector<Engine::Resource<RWorld>*> RWorldVec;
    std::vector<Engine::ManagedInterfacedResource<STDGLRenderer, Window, STDGLWindow>*> WindowVector;
    STDGLModelSystem ModelSystem;
    STDGLShaderSystem ShaderSystem;
    std::unique_ptr<STDGLTextureSystem> TextureSystem; // Unique because it may be replaced while the original one is still needed.
    STDGLMaterialSystem MaterialSystem;

    Engine::UnmanagedInterfacedResource<Renderer, STDGLRenderer>* selfResource; // Stored to be able to make the Engine::Reference objects for objects that it gives out.

    std::array<GLsync, 2> DoubleBufferFences = { nullptr, nullptr };

    GLuint* ModelInstancePreprocessShader;
    GLuint* ModelInstanceReplicatorShader;
    Engine::Reference<STDGLTexture> ErrorATEXForUnset;
    
    uint64_t FrameCounter = 0;

    ImGuiContext* BaseImGuiContext;

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
    void PreprocessIArrays(std::vector<Engine::Reference<STDGLModelInstanceArray>>& InstanceArrayRefs);
    template<bool isDepth, bool isBindless>
    void DrawIArrays(std::vector<Engine::Reference<STDGLModelInstanceArray>>& InstanceArrayRefs);
public:
    ~STDGLRenderer();
    static Engine::Reference<Renderer> Make();
    Engine::Reference<RWorld> MakeRWorld();
    Camera* GetCamera(std::string name);
    const uint64_t& GetFrameCounter();
    void Draw();
    Engine::Reference<Window> MakeWindow(int x, int y, std::string name);
    ImFont* LoadFont(const std::string& path, ImFontConfig* config);
    void UnloadFont(ImFont* Font);
    ImFontAtlas* GetFontAtlas();
    void RendererCommand(std::vector<std::string> Do);
};
