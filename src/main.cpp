#undef AMETHYSTENGINESRC

#include <array>
#include <bit>
#include <GLFW/glfw3.h>
#include "engine/graphics/Camera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "imgui.h"

#include "main.h"
#include "engine/graphics/opengl/STDGLRenderer.h"

#include <iostream>
#include <memory>

#include "engine/master.h"

// Time between current frame and last frame
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

bool EngineShouldNotTerminate = true;

std::function<void(Renderer*, Window*)> mainuifunction = [](Renderer* renderer, Window* window) {

	static bool isUsingCamera = true;

	if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
		isUsingCamera = !isUsingCamera;
		window->EatCursor(isUsingCamera);
	}

	Camera* camera = renderer->GetCamera("cam2");
	float velocity = 100.0f * deltaTime;
	vec3 direction;
	if (isUsingCamera && window->IsWindowInFocus()) {
    	    if (ImGui::IsKeyDown(ImGuiKey_W))
    	        direction  += camera->Front;
    	    if (ImGui::IsKeyDown(ImGuiKey_S))
    	        direction  -= camera->Front;
    	    if (ImGui::IsKeyDown(ImGuiKey_A))
    	        direction  -= camera->Right;
    	    if (ImGui::IsKeyDown(ImGuiKey_D))
    	        direction  += camera->Right;
    	    if (ImGui::IsKeyDown(ImGuiKey_Space))
    	        direction  += vec3(0, 0, 1);
    	    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    	        direction += vec3(0, 0, -1);
		direction = direction.norm();
		camera->Position += direction * velocity;
		static vec2 lastmouse = vec2(0, 0);
		vec2 currmouse = std::bit_cast<vec2>(ImGui::GetMousePos());
		if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
			lastmouse = currmouse;
		vec2 mouseoffset = currmouse - lastmouse;
		lastmouse = currmouse;
		camera->ProcessMouseMovement(mouseoffset, true);
	}

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

	//Draws the camera output
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("main", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoBackground);
		ImVec2 CameraSize = ImGui::GetContentRegionAvail();
		ImGui::Image(camera->GetTexture(), CameraSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::PopStyleVar(3);

	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		EngineShouldNotTerminate = false;
	}
	ImGui::Begin("Hello from ui function");
		ImGui::Text("Delta		 : %f", deltaTime);
		ImGui::Text("Camera pitch: %f", camera->Pitch);
		ImGui::Text("Camera yaw  : %f", camera->Yaw);
		ImGui::Text("Camera X	 : %f", camera->Position.x);
		ImGui::Text("Camera Y	 : %f", camera->Position.y);
		ImGui::Text("Camera Z	 : %f", camera->Position.z);
	ImGui::End();
	//ImGui::ShowDemoWindow();
};

int main() {
	glfwInit();

	std::shared_ptr openglrenderer = STDGLRenderer::Make();
	std::shared_ptr<Window> enginewindow = openglrenderer->MakeWindow();
	enginewindow->Name = "Amethyst";
	enginewindow->Update();
	enginewindow->SetUIFunction(mainuifunction);
	enginewindow->EatCursor(true);
	auto rworld = openglrenderer->MakeRWorld();
	std::array<std::shared_ptr<Camera>, 2> cameras;
	cameras[0] = rworld->MakeCamera(vec2(800, 600), "cam1");
	cameras[1] = rworld->MakeCamera(vec2(800 * 4, 600 * 4), "cam2", vec3(1, 1, 1));
	std::array<std::unique_ptr<ModelInstance>, 2> models;
	models[0] = rworld->MakeModelInstance("sphere.glb");
	models[0]->SetMatrix(mat4());
	models[1] = rworld->MakeModelInstance("sphere.glb");
	models[1]->SetMatrix(mat4(1, 0, 0, 128));

	std::cout << "Hello, world!" << std::endl;

	bool isWindowOpen = true;
	while(EngineShouldNotTerminate) {
		float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		openglrenderer->Draw();
		
		glfwPollEvents();    
	}

	glfwTerminate();

}
