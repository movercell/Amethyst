#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <utility>
#include "GLFW/glfw3.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "engine/graphics/Window.h"
#include "STDGLRenderer.h"
#include "STDGLRWorld.h"
#include "GLMisc.h"
#include "STDGLWindow.h"

#include "../src/shader.h"

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

    GLFWwindow* data = glfwCreateWindow(1, 1, "The “onosecond” is the second after you make a terrible mistake. The second when you realise what you just did", NULL, NULL);
    glfwMakeContextCurrent(data);

    glfwSwapInterval(1); // TODO: add a vsync setting

    rendererData = data;

    ShaderSystem.Init();

    ModelInstancePreprocessShader = ShaderSystem.GetComputeShader("STDGLModel_InstancePreprocess");
    ModelInstanceReplicatorShader = ShaderSystem.GetComputeShader("STDGLModel_InstanceReplicator");
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

    auto tmpshader = ShaderSystem.GetShaderPipeline("Generic", "UnlitGeneric").first;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);

    for (auto rworldres : RWorldVec) {

        auto rworld = static_cast<STDGLRWorld*>(rworldres->Get());
        if (rworld->isSkippingRendering())
            continue;

        // Get references to all instance arrays.
        std::vector<Engine::Reference<STDGLModelInstanceArray>> InstanceArrayRefs;
        InstanceArrayRefs.reserve(rworld->InstanceArrays.size());
        for (auto& [_, iarray] : rworld->InstanceArrays)
            InstanceArrayRefs.emplace_back(iarray);

        for (auto camera : rworld->CameraVec) {

            GL_PUSH_DEBUG(camera->resource.Name.c_str());
            camera->resource.Bind();
            glViewport(0, 0, camera->resource.GetResolution().x, camera->resource.GetResolution().y);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            
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

            // Draw.
            glUseProgram(0);
            glBindProgramPipeline(tmpshader);
            for (auto& iarray : InstanceArrayRefs) {
                iarray->Bind();
                iarray->Model->BindInfo();
                iarray->Model->BindIndirectCommands();

                iarray->Model->Draw();
            }

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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

Engine::Reference<RWorld> STDGLRenderer::MakeRWorld() {
    auto result = new Engine::ManagedInterfacedResource<STDGLRenderer, RWorld, STDGLRWorld>(this, selfResource, rendererData, &ModelSystem);
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
