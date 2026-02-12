#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <memory>
#include <utility>
#include "GLFW/glfw3.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "engine/graphics/Window.h"
#include "engine/graphics/opengl/STDGLRenderer.h"
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

    GraphicsMisc::windowSetCallbacks(reinterpret_cast<GLFWwindow*>(window));

    return data;
}

void STDGLRenderer::UIDeleteData(____UIData* data) {
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(data));

    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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
    glEnable(GL_CULL_FACE);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);

    Shader shader = Shader("scripts/shaders/opengl/generic.vs", "scripts/shaders/opengl/generic.fs");
    STDGLModel tmpmodel = STDGLModel("error.glb");
    

    auto SharedRWorldVec = RWorldVec.lock();
    for (auto rworldbase : SharedRWorldVec) {
        auto rworld = static_pointer_cast<STDGLRWorld>(rworldbase);
        auto SharedCameraVec = rworld->CameraVec.lock();
        for (std::shared_ptr<STDGLCamera>& camera : SharedCameraVec) {
            camera->Bind(CameraUBO);
            glViewport(0, 0, camera->GetResolution().x, camera->GetResolution().y);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            shader.use();
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, rworld->tmpinstancearr.InstanceBuffer);
            tmpmodel.Draw();
        }
        
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Draw windows.
    auto SharedWindowVector = WindowVector.lock();
    for (auto window : SharedWindowVector) {
        glViewport(0, 0, 800, 600);
        window->Draw();
    }
    
}

std::shared_ptr<RWorld> STDGLRenderer::MakeRWorld() {
    auto result = make_shared<STDGLRWorld>(selfRef);
    RWorldVec.push_back(result);

    return result;
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
