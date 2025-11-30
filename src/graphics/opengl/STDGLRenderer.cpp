#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <memory>
#include <utility>
#include "GLFW/glfw3.h"
#include "graphics/RWorld.h"
#include "graphics/Renderer.h"
#include "graphics/Window.h"
#include "graphics/opengl/STDGLRenderer.h"
#include "STDGLRWorld.h"
#include "../misc.h"

#include "../../src/model.h"
#include "../../src/shader.h"

std::shared_ptr<Renderer> STDGLRenderer::Make() {

    std::shared_ptr<STDGLRenderer> tempRendererRef = std::make_shared<STDGLRenderer>();

    tempRendererRef->selfRef = std::static_pointer_cast<Renderer>(tempRendererRef);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* data = glfwCreateWindow(1, 1, "The “onosecond” is the second after you make a terrible mistake. The second when you realise what you just did", NULL, NULL);
    glfwMakeContextCurrent(data);

    tempRendererRef->rendererData = reinterpret_cast<____WindowData*>(data);

    glCreateBuffers(1, &(tempRendererRef->CameraUBO));

    glfwDefaultWindowHints();
    return tempRendererRef;
}

STDGLRenderer::~STDGLRenderer() {
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(rendererData));
}

____UIData* STDGLRenderer::UINewData(____WindowData* window) {
    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	____UIData* data = reinterpret_cast<____UIData*>(ImGui::CreateContext());
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(data));
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(window), false);
    ImGui_ImplOpenGL3_Init();

    glfwSetWindowUserPointer(reinterpret_cast<GLFWwindow*>(window), reinterpret_cast<void*>(data));
    GraphicsMisc::windowSetCallbacks(reinterpret_cast<GLFWwindow*>(window));

    return data;
}

void STDGLRenderer::UINewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void STDGLRenderer::UIEndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void STDGLRenderer::Draw() {
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(rendererData));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, CameraUBO);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    Shader shader = Shader("scripts/shaders/opengl/generic.vs", "scripts/shaders/opengl/generic.fs");
    Model tmpmodel = Model("sphere.glb");
    

    for (int RWorldI = 0; RWorldI < RWorldVec.size(); RWorldI++) {
        STDGLRWorld* rworld = static_cast<STDGLRWorld*>(RWorldVec[RWorldI]);
        for (int CameraI = 0; CameraI < rworld->CameraVec.size(); CameraI++) {
            const std::shared_ptr<STDGLCamera>& camera = rworld->CameraVec[CameraI].lock();

            // Check whether the camera still exists.
            if (!camera) {
                CameraI--;
                rworld->CameraVec.erase(rworld->CameraVec.begin() + CameraI);
                rworld->CameraVec.pop_back();
                continue;
            }

            camera->Bind(CameraUBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            shader.use();
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, rworld->tmpinstancearr.InstanceBuffer);
            tmpmodel.Draw();

        }
        
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Draw windows.
    for (int i = 0; i < windowVector.size(); i++) {
        std::weak_ptr<____Window>& window = windowVector[i];
        if (window.expired()) {
            i--;
            windowVector.erase(windowVector.begin() + i);
            continue;
        }
        glViewport(0, 0, 800, 600);
        window.lock()->Draw();
    }
    
}

RWorld* STDGLRenderer::newRWorld() {
    STDGLRWorld* result = new STDGLRWorld(selfRef);
    RWorldVec.push_back(result);

    return result;
}

void STDGLRenderer::deleteRWorld(RWorld* target) {
    auto location = std::find(RWorldVec.begin(), RWorldVec.end(), target);
    std::swap(RWorldVec[std::distance(RWorldVec.begin(), location)], RWorldVec.back());
    RWorldVec.pop_back();
}

Camera* STDGLRenderer::GetCamera(std::string name) {
    Camera* result = nullptr;
    for (RWorld* rworld : RWorldVec) {
        auto temp = rworld->GetCamera(name);
        if (temp) result = temp;
    }
    return result;
}
