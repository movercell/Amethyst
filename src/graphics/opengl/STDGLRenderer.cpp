#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <utility>
#include "GLFW/glfw3.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "engine/graphics/Window.h"
#include "engine/graphics/opengl/STDGLRenderer.h"
#include "STDGLRWorld.h"
#include "GLMisc.h"
#include "STDGLWindow.h"

#include "../../src/shader.h"

std::shared_ptr<Renderer> STDGLRenderer::Make() {
    GLMisc::EnsureGLLoaded();

    std::shared_ptr<STDGLRenderer> tempRendererRef = std::make_shared<STDGLRenderer>();

    tempRendererRef->selfRef = std::static_pointer_cast<Renderer>(tempRendererRef);

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* data = glfwCreateWindow(1, 1, "The “onosecond” is the second after you make a terrible mistake. The second when you realise what you just did", NULL, NULL);
    glfwMakeContextCurrent(data);

    tempRendererRef->rendererData = data;

    tempRendererRef->ShaderSystem.Init();

    tempRendererRef->ModelIndirectReplicationShader = 
                    tempRendererRef->ShaderSystem.GetComputeShader("STGLModel_IndirectBufferReplicator");
    tempRendererRef->ModelInstancePreprocessShader = 
                    tempRendererRef->ShaderSystem.GetComputeShader("STDGLModel_InstancePreprocess");

    glfwDefaultWindowHints();
    return tempRendererRef;
}

STDGLRenderer::~STDGLRenderer() {
    glfwDestroyWindow(rendererData);
}


std::shared_ptr<Window> STDGLRenderer::MakeWindow() {
    std::shared_ptr<STDGLWindow> tempRef = std::make_shared<STDGLWindow>(selfRef, rendererData);
    WindowVector.push_back(tempRef);
    return static_pointer_cast<Window>(tempRef);
}


void STDGLRenderer::Draw() {
    glfwMakeContextCurrent(rendererData);

    bool isFrameOdd = Engine::FrameCount & 1;

    if (DoubleBufferFences[isFrameOdd]) {
        glClientWaitSync(DoubleBufferFences[isFrameOdd], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        glDeleteSync(DoubleBufferFences[isFrameOdd]);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);

    Shader tmpshader = Shader("scripts/shaders/opengl/generic.vs", "scripts/shaders/opengl/generic.fs");

    auto SharedRWorldVec = RWorldVec.lock();
    for (auto& rworldbase : SharedRWorldVec) {

        auto rworld = static_pointer_cast<STDGLRWorld>(rworldbase);
        if (rworld->isSkippingRendering())
            continue;
        auto SharedCameraVec = rworld->CameraVec.lock();
        auto SharedInstanceArraysVec = rworld->InstanceArrays.lock();

        // Flush the writes
        for (auto& iarray : SharedInstanceArraysVec) {
            glFlushMappedNamedBufferRange(iarray->InstanceBuffer, 
                    sizeof(STDGLModelInstanceArray::InstanceArrayBuffer) * isFrameOdd,
                    sizeof(STDGLModelInstanceArray::InstanceArrayBuffer));
        }

        for (std::shared_ptr<STDGLCamera>& camera : SharedCameraVec) {

            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, camera->Name.c_str());
            camera->Bind();
            glViewport(0, 0, camera->GetResolution().x, camera->GetResolution().y);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            

            for (auto& iarray : SharedInstanceArraysVec) {
                glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, iarray->InstanceBuffer,
                        sizeof(STDGLModelInstanceArray::InstanceArrayBuffer) * isFrameOdd,
                        sizeof(STDGLModelInstanceArray::InstanceArrayBuffer));
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, iarray->Model->ModelInfo);
                glUseProgram(ModelInstancePreprocessShader);
                glDispatchCompute(STDGLMODEL_INSTANCE_MAX_COUNT / 128, 1, 1);
            }

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            for (auto& iarray : SharedInstanceArraysVec) {
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, iarray->Model->ModelInfo);
                glUseProgram(ModelIndirectReplicationShader);
                glDispatchCompute(1, 1, 1);
            }

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

            for (auto& iarray : SharedInstanceArraysVec) {
                glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, iarray->InstanceBuffer,
                        sizeof(STDGLModelInstanceArray::InstanceArrayBuffer) * isFrameOdd,
                        sizeof(STDGLModelInstanceArray::InstanceArrayBuffer));
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, iarray->Model->ModelInfo);
                glBindBuffer(GL_DRAW_INDIRECT_BUFFER, iarray->Model->ModelInfo);
                glBindVertexArray(iarray->Model->VAO);
                tmpshader.use();

                iarray->Model->Draw();
            }

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            glPopDebugGroup();
            
        }
        
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Draw windows.
    auto SharedWindowVector = WindowVector.lock();
    for (auto& window : SharedWindowVector) {
        window->Draw();
    }

    DoubleBufferFences[isFrameOdd] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    
}

std::shared_ptr<RWorld> STDGLRenderer::MakeRWorld() {
    auto result = make_shared<STDGLRWorld>(selfRef, rendererData, &ModelSystem);
    RWorldVec.push_back(result);

    return static_pointer_cast<RWorld>(result);
}

Camera* STDGLRenderer::GetCamera(std::string name) {
    Camera* result = nullptr;
    auto SharedRWorldVec = RWorldVec.lock();
    for (auto rworld : SharedRWorldVec) {
        auto temp = rworld->GetCamera(name);
        if (temp) result = temp;
    }
    return result;
}
