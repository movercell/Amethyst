#ifndef _WIN32
	#undef AMETHYSTENGINESRC
#endif

#include <array>
#include <bit>
#include <GLFW/glfw3.h>
#include "engine/graphics/Camera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "imgui.h"
#include "engine/filesystem/Filesystem.h"

#include "main.h"
#include "Init.h"

#include <iostream>
#include <memory>
#include <cstdint>

#include "engine/master.h"

#include "engine/filesystem/ADF.h"
#include "engine/entities/Entity.h"

// Time between current frame and last frame
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

bool EngineShouldNotTerminate = true;

namespace Engine {
	
}

std::array<std::unique_ptr<ModelInstance>, 4> models;
std::vector<std::unique_ptr<ModelInstance>> extramodels;

std::function<void(Renderer*, Window*)> mainuifunction = [](Renderer* renderer, Window* window) {

	static bool isUsingCamera = false;
	static float Pitch = 0.0f;
	static float Yaw = 0.0f;
	static vec3 CameraPosition = vec3(1.0f, 1.0f, 1.0f);

	if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
		isUsingCamera = !isUsingCamera;
		window->SetEatCursor(isUsingCamera);
	}

	Camera* camera = renderer->GetCamera("cam2");
	float velocity = 100.0f * deltaTime;
	vec3 direction;
	if (isUsingCamera && window->IsWindowInFocus()) {
    	    if (ImGui::IsKeyDown(ImGuiKey_W))
    	        direction += camera->GetFront();
    	    if (ImGui::IsKeyDown(ImGuiKey_S))
    	        direction -= camera->GetFront();
    	    if (ImGui::IsKeyDown(ImGuiKey_A))
    	        direction += camera->GetLeft();
    	    if (ImGui::IsKeyDown(ImGuiKey_D))
    	        direction -= camera->GetLeft();
    	    if (ImGui::IsKeyDown(ImGuiKey_Space))
    	        direction += vec3(0, 0, 1);
    	    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    	        direction -= vec3(0, 0, 1);
		direction = direction.norm();
		CameraPosition += direction * velocity;


		static vec2 lastmouse = vec2(0, 0);
		vec2 currmouse = std::bit_cast<vec2>(ImGui::GetMousePos());
		if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
			lastmouse = currmouse;
		vec2 mouseoffset = currmouse - lastmouse;
		lastmouse = currmouse;
		
		mouseoffset.x *= 7.5f * deltaTime;
    	mouseoffset.y *= 7.5f * deltaTime;
    	Yaw   -= mouseoffset.x;
    	Pitch += mouseoffset.y;

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
	}
	camera->SetPosition(CameraPosition);
	camera->SetAngles(vec3(Pitch, Yaw));

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
		ImGui::Text("Camera pitch: %f", Pitch);
		ImGui::Text("Camera yaw  : %f", Yaw);
		ImGui::Text("Camera X	 : %f", CameraPosition.x);
		ImGui::Text("Camera Y	 : %f", CameraPosition.y);
		ImGui::Text("Camera Z	 : %f", CameraPosition.z);
		if (ImGui::Button("Delete model 0"))
			models[0].reset();
	ImGui::End();
	//ImGui::ShowDemoWindow();
};

int main() {
	Engine::Init();

	Engine::Reference<Renderer> openglrenderer = Renderer::Make("STDGLRenderer");
	auto rworld = openglrenderer->MakeRWorld();

	World tmpworld(rworld);
	auto savefile = ADFEntry::FromFile("saves/testsave.adf");
	tmpworld.Restore(savefile);

	auto newsavefile = tmpworld.Save();
	newsavefile.ToFile("saves/hi.adf");
	newsavefile.ToFile("saves/hicompacted.adf", true);

	auto tmpentityhandler = tmpworld[0];

	Engine::Reference<Window> enginewindow = openglrenderer->MakeWindow(800, 600, "Amethyst");
	enginewindow->SetUIFunction(mainuifunction);
	std::array<Engine::Reference<Camera>, 2> cameras;
	//cameras[0] = rworld->MakeCamera(vec2(800, 600), "cam1");
	cameras[1] = rworld->MakeCamera(vec2(800 * 2, 600 * 2), "cam2");
	models[0] = rworld->MakeModelInstance("multimesh.adf");
	models[1] = rworld->MakeModelInstance(".glb");
	models[2] = rworld->MakeModelInstance("cube.adf");
	models[3] = rworld->MakeModelInstance("floor_plane.adf");

	std::array<Engine::Reference<Light>, 2> lights;
	lights[0] = rworld->MakeSpotLight(vec2(2048, 2048), 55, 60, vec3(1.0f, 1.0f, 1.0f), 1, 512.0);
	lights[0]->SetPosition(vec3(58.0f, 77.0f, 65.0f));
	lights[0]->SetAngles(vec3(31.0f, -120.0f, 0.0f));
	lights[0]->SetFalloff(0.0f, 0.7f, 0.1f);
	lights[0]->SetIntensity(79999.0f);

	lights[1] = rworld->MakeSpotLight(vec2(1024, 1024), 55, 60, vec3(0.0f, 1.0f, 0.0f), 1, 256.0);
	lights[1]->SetPosition(vec3(-88.0f, 20.0f, 37.0f));
	lights[1]->SetAngles(vec3(20.0f, -30.0f, 0.0f));
	lights[1]->SetFalloff(0.0f, 0.3f, 0.7f);
	lights[1]->SetIntensity(9999.0f);


	extramodels.reserve(128);
	for (int i = 0; i < 128; i++) {
		extramodels.push_back(rworld->MakeModelInstance(std::to_string(i)));
	}

	std::cout << "Hello, world!" << std::endl;

	auto datafilething = ADFEntry::FromFile("scripts/shaders/glshaders.adf");

	while(EngineShouldNotTerminate) {
		float currentFrame = static_cast<float>(glfwGetTime());
		static float position = 0;
        float rawdeltaTime = currentFrame - lastFrame;
		deltaTime = std::lerp(deltaTime, rawdeltaTime, 0.1f);
        lastFrame = currentFrame;

		if (models[0])
			models[0]->SetMatrix(mat4(1, 0, 0, -128));
		models[1]->SetMatrix(mat4());
		models[3]->SetMatrix(mat4());

		models[2]->SetMatrix(quat(vec3(0, position, 0)).MakeRotationMatrix() * mat4(10, 0, 0, -64,
																					0, 10, 0, 0,
																					0, 0, 10, 10,
																					0, 0, 0, 1));
		position += 32.0f * deltaTime;
		if (position > 360.0f) position -= 360.0f;

		float x = 128.0f;
		for (auto& model : extramodels) {
			model->SetMatrix(mat4(1.0f, 0.0f, 0.0f, 0.0f,
			 					  0.0f, 1.0f, 0.0f, -x,
			 					  0.0f, 0.0f, 1.0f, 0.0f,
			 					  0.0f, 0.0f, 0.0f, 1.0f));
			x += 128.0f;
		}

		tmpworld.Update();

		openglrenderer->Draw();
		
		glfwPollEvents();    
	}

	for(auto& model : models) model.reset();
	for(auto& model : extramodels) model.reset();
}