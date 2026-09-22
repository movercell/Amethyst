#include <array>
#include <bit>
#include "engine/graphics/Camera.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "imgui.h"
#include "engine/filesystem/Filesystem.h"

#include <iostream>
#include <memory>
#include <cstdint>

#include "engine/master.h"

#include "engine/filesystem/ADF.h"
#include "engine/entities/Entity.h"

#include "entities/player.h"

Engine::Reference<Renderer> renderer;
Engine::Reference<RWorld> rworld;
Engine::Reference<Window> window;
std::array<std::unique_ptr<ModelInstance>, 4> models;
std::vector<std::unique_ptr<ModelInstance>> extramodels;
std::array<Engine::Reference<Camera>, 2> cameras;
std::array<Engine::Reference<Light>, 2> lights;
Engine::Reference<World> world;

std::optional<ADFEntry> QueuedLoad;

enum class GameState {
	Normal,
	Loading,
	Paused
};
GameState CurrentGameState = GameState::Normal;


constexpr float MainMenuXOffsetRatio = 0.05f;
constexpr float MainMenuYOffsetRatio = 0.5f;
constexpr float MainMenuWidthRatio = 0.35f;
constexpr float MainMenuButtonHeightRatio = 0.15f;
constexpr float MainMenuButtonStylishBarWidthRatio = 0.05f;
constexpr ImColor MainMenuButtonStylishBarHoveredColor = ImColor(153, 102, 204, 255);
constexpr ImColor MainMenuButtonStylishBarNotHoveredColor = ImColor(53, 2, 104, 255);
constexpr float MainMenuButtonTextOffset = 0.02f;

static bool MainMenuButton(const char* Label, float MainMenuWidth) {
	bool result = ImGui::Button(Label, ImVec2(-1.0f, MainMenuWidth * MainMenuButtonHeightRatio));

	ImDrawList* DrawList = ImGui::GetWindowDrawList();
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 size = ImGui::GetItemRectSize();
	size.x *= MainMenuButtonStylishBarWidthRatio;
	ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);
	bool isHovered = ImGui::IsItemHovered();
	DrawList->AddRectFilled(min, max, isHovered ? MainMenuButtonStylishBarHoveredColor : MainMenuButtonStylishBarNotHoveredColor);

	return result;
}

std::function<void(Renderer*, Window*)> mainuifunction = [](Renderer* renderer, Window* window) {

	// Loading screen stuff.
	static float CurrentLoadingPopupAlpha = 0.0f;
	static const char* CurrentLoadingPopupText = "";
	bool IsLoading = CurrentGameState == GameState::Loading;
	if (IsLoading)
		CurrentLoadingPopupAlpha = 1.0f;
	if (CurrentLoadingPopupAlpha > 0.0f) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, CurrentLoadingPopupAlpha);
		// They are separately named so that their sizes are fit to their text.(If ImGuiWindowFlags_AlwaysAutoResize is set then there's still a frame of wrong size.)
		ImGui::Begin(CurrentLoadingPopupText, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		ImGui::TextUnformatted(CurrentLoadingPopupText);
		ImGui::End();
		ImGui::PopStyleVar();
		CurrentLoadingPopupAlpha -= deltaTime * 1.5f;
	}
	if (IsLoading)
		return; // No need to draw the rest of the UI while the game is loading as it'll look very broken.


	// Pause logic.
	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
		if (CurrentGameState == GameState::Normal) {
			CurrentGameState = GameState::Paused;
		} else if (CurrentGameState == GameState::Paused) {
			CurrentGameState = GameState::Normal;
		}
	}

	// Main/pause menu
	if (CurrentGameState == GameState::Paused) {
		ImGui::SetNextWindowPos(ImVec2(window->GetWidth() * MainMenuXOffsetRatio, window->GetHeight() * MainMenuYOffsetRatio));
		float MainMenuWidth = window->GetWidth() * MainMenuWidthRatio;
		ImGui::SetNextWindowSize(ImVec2(MainMenuWidth, -1.0f));
		ImGui::Begin("Main menu buttons", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(MainMenuButtonStylishBarWidthRatio + MainMenuButtonTextOffset, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

		if (MainMenuButton("Resume", MainMenuWidth)) {
			CurrentGameState = GameState::Normal;
		}
		if (MainMenuButton("Quit", MainMenuWidth)) {
			Engine::QueueShutdown();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
	
	// In-game.
	static bool shouldUseCamera = false;

	if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
		shouldUseCamera = !shouldUseCamera;
	}
	static char previsUsingCamera = 2; // So that it's not equal by default.
	char isUsingCamera = shouldUseCamera && CurrentGameState == GameState::Normal;
	if (isUsingCamera != previsUsingCamera) {
		previsUsingCamera = isUsingCamera;
		window->SetEatCursor(isUsingCamera);
	}

	auto PlayerEntityHandler = (*world)[0];
	if (!PlayerEntityHandler || PlayerEntityHandler->GetClassname() != "player") return;
	Entity_Player* PlayerEntity = reinterpret_cast<Entity_Player*>(PlayerEntityHandler->GetEntityPtr());

	Engine::Reference<Camera> camera = PlayerEntity->PlayerCamera;

	// Always calculate the mouse offset because the controls logic CAN get skipped.
	static vec2 lastmouse = vec2(0, 0);
	vec2 currmouse = std::bit_cast<vec2>(ImGui::GetMousePos());
	if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
		lastmouse = currmouse;
	vec2 mouseoffset = currmouse - lastmouse;
	lastmouse = currmouse;

	mouseoffset.x *= 7.5f * deltaTime;
	mouseoffset.y *= 7.5f * deltaTime;

	// Camera controls.
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
		PlayerEntity->position += direction * velocity;

    	PlayerEntity->yaw   -= mouseoffset.x;
    	PlayerEntity->pitch += mouseoffset.y;

        if (PlayerEntity->pitch > 89.0f)
            PlayerEntity->pitch = 89.0f;
        if (PlayerEntity->pitch < -89.0f)
            PlayerEntity->pitch = -89.0f;
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

	if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
		Engine::QueueShutdown();
	}
	ImGui::Begin("Hello from ui function");
		ImGui::Text("Delta		 : %f", deltaTime);
		ImGui::Text("Player pitch: %f", PlayerEntity->pitch);
		ImGui::Text("Player yaw  : %f", PlayerEntity->yaw);
		ImGui::Text("Player X	 : %f", PlayerEntity->position.x);
		ImGui::Text("Player Y	 : %f", PlayerEntity->position.y);
		ImGui::Text("Player Z	 : %f", PlayerEntity->position.z);
		if (ImGui::Button("Delete model 0")) {
			models[0].reset();
		}
		if (ImGui::Button("Quicksave(F5)") || ImGui::IsKeyPressed(ImGuiKey_F5, false)) {
			CurrentLoadingPopupText = "Saved!";
			CurrentLoadingPopupAlpha = 1.0f;
			world->Save().ToFile("saves/quick.adf", true);
		}
		if (ImGui::Button("Quickload(F6)") || ImGui::IsKeyPressed(ImGuiKey_F6, false)) {
			CurrentLoadingPopupText = "Loading...";
			QueuedLoad = ADFEntry::FromFile("saves/quick.adf");
		}

	ImGui::End();
};

void gameinit() {
	Engine::RegisterDefaultEngineEntityTypes();
	
	Engine::RegisterEntityClass<Entity_Player>();
	Engine::RegisterEntityClass<Entity_PlayerStart>();

	renderer = Renderer::Make("STDGLRenderer");
	rworld = renderer->MakeRWorld();

	world = World::Make("Primary", rworld);
	auto savefile = ADFEntry::FromFile("saves/testsave.adf");
	world->Load(savefile);

	auto newsavefile = world->Save();
	newsavefile.ToFile("saves/hi.adf");
	newsavefile.ToFile("saves/hicompacted.adf", true);

	auto tmpentityhandler = (*world)[0];

	window = renderer->MakeWindow(800, 600, "Amethyst");
	window->SetUIFunction(mainuifunction);
	models[0] = rworld->MakeModelInstance("multimesh.adf");
	models[1] = rworld->MakeModelInstance(".glb");
	models[2] = rworld->MakeModelInstance("cube.adf");
	models[3] = rworld->MakeModelInstance("floor_plane.adf");

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
		extramodels.push_back(rworld->MakeModelInstance("backpack.adf"));
	}

	std::cout << "Hello, world!(From game dll!)" << std::endl;

}



static void GameStateHandler_Normal() {
	static float position = 0;
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

	world->Update();

	renderer->Draw();

	if (QueuedLoad) {
		CurrentGameState = GameState::Loading;
	}
}


static void GameStateHandler_Loading() {
	world->Load(QueuedLoad.value());
	QueuedLoad.reset();
	// Need to update and draw normally at least once so that everything is set up correctly.
	GameStateHandler_Normal();

	CurrentGameState = GameState::Normal;
}


static void GameStateHandler_Paused() {
	renderer->Draw();
}


void gameloop() {
	switch (CurrentGameState) {
	case GameState::Normal:
		GameStateHandler_Normal();
		break;
	case GameState::Loading:
		GameStateHandler_Loading();
		break;
	case GameState::Paused:
		GameStateHandler_Paused();
		break;
	}
}