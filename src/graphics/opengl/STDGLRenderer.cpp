#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <utility>
#include "GLFW/glfw3.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "engine/graphics/Window.h"
#include "engine/Resource.h"
#include "STDGLRenderer.h"
#include "STDGLRWorld.h"
#include "GLMisc.h"
#include "STDGLWindow.h"

Engine::Reference<Renderer> STDGLRenderer::Make() {
    GLMisc::EnsureGLLoaded();

    auto tempres = new Engine::UnmanagedInterfacedResource<Renderer, STDGLRenderer>();

    tempres->resource.selfResource = tempres;

    tempres->resource.Init();

    return Engine::Reference(tempres);
}

void STDGLRenderer::Init() {
    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#ifndef DEBUG
    // TODO: Should add a setting to make it override NO_ERROR even in release
    glfwWindowHint(GLFW_NO_ERROR, GLFW_TRUE);
#endif

    GLFWwindow* data = glfwCreateWindow(1, 1, "The “onosecond” is the second after you make a terrible mistake. The second when you realise what you just did", nullptr, nullptr);
    glfwMakeContextCurrent(data);

    glfwSwapInterval(1); // TODO: add a vsync setting

    rendererData = data;

    ShaderSystem.Init();

    ModelInstancePreprocessShader = ShaderSystem.GetComputeShader("STDGLModel_InstancePreprocess");
    ModelInstanceReplicatorShader = ShaderSystem.GetComputeShader("STDGLModel_InstanceReplicator");


    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearDepth(0.0f);
    glClearColor(0, 0, 0, 1);
    glDepthFunc(GL_GREATER);
}

STDGLRenderer::~STDGLRenderer() {
    glfwDestroyWindow(rendererData);
}


Engine::Reference<Window> STDGLRenderer::MakeWindow(int x, int y, std::string name) {
    auto res = new Engine::ManagedInterfacedResource<STDGLRenderer, Window, STDGLWindow>(this, selfResource, rendererData, x, y, name);
    WindowVector.push_back(res);
    return Engine::Reference(res);
}


void STDGLRenderer::Draw() {
    glfwMakeContextCurrent(rendererData);

    bool isFrameOdd = FrameCounter & 1;

    if (DoubleBufferFences[isFrameOdd]) {
        glClientWaitSync(DoubleBufferFences[isFrameOdd], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        glDeleteSync(DoubleBufferFences[isFrameOdd]);
    }

    glEnable(GL_DEPTH_TEST);

    for (auto rworldres : RWorldVec) {

        auto rworld = static_cast<STDGLRWorld*>(rworldres->Get());
        if (rworld->isSkippingRendering())
            continue;

        rworld->lightsystem.Bind();

        // Get references to all instance arrays.
        std::vector<Engine::Reference<STDGLModelInstanceArray>> InstanceArrayRefs;
        InstanceArrayRefs.reserve(rworld->InstanceArrays.size());
        for (auto& [_, iarray] : rworld->InstanceArrays)
            InstanceArrayRefs.emplace_back(iarray);
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(5.0f, 2.0f);
        glDisable(GL_CULL_FACE);
        for (auto light : rworld->lightsystem.LightResources) {
            if (!light) continue;
            
            GL_PUSH_DEBUG("Light");
            light->resource.Bind();
            glViewport(light->resource.TextureSpace.PosX, light->resource.TextureSpace.PosY, light->resource.TextureSpace.SizeX, light->resource.TextureSpace.SizeY);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            PreprocessIArrays(InstanceArrayRefs);
            DrawIArrays<true>(InstanceArrayRefs);

            GL_POP_DEBUG;
        }

        glDisable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_CULL_FACE);
        for (auto camera : rworld->CameraVec) {

            GL_PUSH_DEBUG(camera->resource.Name.c_str());
            camera->resource.Bind();
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // Instance culling.
            PreprocessIArrays(InstanceArrayRefs);
            
            // Z-Prepass.
            DrawIArrays<true>(InstanceArrayRefs);

            // TODO: Cluster processing

            // Normal rendering.
            glDepthFunc(GL_EQUAL);
            DrawIArrays<false>(InstanceArrayRefs);
            glDepthFunc(GL_GREATER);

            GL_POP_DEBUG;
            
        }
        
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FrameCounter++;

    // Draw windows.
    for (auto& window : WindowVector) {
        window->resource.Draw();
    }

    DoubleBufferFences[isFrameOdd] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    
}

void STDGLRenderer::PreprocessIArrays(std::vector<Engine::Reference<STDGLModelInstanceArray>>& InstanceArrayRefs) {
    // Clear the instance counts.
    for (auto& iarray : InstanceArrayRefs) {
        auto* model = iarray->Model.get();

        for (int i = 0; i < model->LODCount; i++) {
            glClearNamedBufferSubData(model->ModelInfo, GL_R32UI, i * STDGLMODEL_MESH_MAX_COUNT * sizeof(DrawElementsIndirectCommand) + offsetof(DrawElementsIndirectCommand, instanceCount), sizeof(GLuint), GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
        }
    }

    // Cull instances.
    glUseProgram(ModelInstancePreprocessShader);
    for (auto& iarray : InstanceArrayRefs) {
        iarray->Bind();
        iarray->Model->BindInfo();
        int groupcount = iarray->NextIndex / STDGLMODEL_INSTANCE_PREPROCESS_GROUP_SIZE;
        glDispatchCompute(iarray->NextIndex % STDGLMODEL_INSTANCE_PREPROCESS_GROUP_SIZE ? groupcount + 1 : groupcount, 1, 1);
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Replicate the instance counts across all LOD meshes.
    glUseProgram(ModelInstanceReplicatorShader);
    for (auto& iarray : InstanceArrayRefs) {
        iarray->Model->BindInfo();
        glDispatchCompute(1, 1, 1);
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
}

template<bool isDepth>
void STDGLRenderer::DrawIArrays(std::vector<Engine::Reference<STDGLModelInstanceArray>>& InstanceArrayRefs) {
    // Draw.
    glUseProgram(0);
    GLuint tmpshader;
    if constexpr (isDepth)
        tmpshader = ShaderSystem.GetShaderPipeline("Generic", "Generic").second;
    else
        tmpshader = ShaderSystem.GetShaderPipeline("Generic", "Generic").first;
    glBindProgramPipeline(tmpshader);
    for (auto& iarray : InstanceArrayRefs) {
        iarray->Bind();
        
        auto& Model = iarray->Model;
        Model->BindInfo();
        Model->BindIndirectCommands();

        Model->Draw<isDepth>();
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}



Engine::Reference<RWorld> STDGLRenderer::MakeRWorld() {
    glfwMakeContextCurrent(rendererData);
    auto result = new Engine::ManagedInterfacedResource<STDGLRenderer, RWorld, STDGLRWorld>(this, selfResource, &ModelSystem);
    result->resource.selfResource = result;
    RWorldVec.push_back(result);

    return Engine::Reference(result);
}

Camera* STDGLRenderer::GetCamera(std::string name) {
    Camera* result = nullptr;
    for (auto rworld : RWorldVec) {
        auto temp = rworld->Get()->GetCamera(name);
        if (temp) result = temp;
    }
    return result;
}

const uint64_t& STDGLRenderer::GetFrameCounter() {
    return FrameCounter;
}
