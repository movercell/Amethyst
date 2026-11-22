#include <array>
#include <bit>
#include "engine/Filesystem.h"

#include <iostream>
#include <memory>
#include <cstdint>

#include "engine/master.h"

#include "main.h"
#include "ui.h"

#include "engine/Console.h"

Engine::Reference<Renderer> renderer;
Engine::Reference<RWorld> rworld;
Engine::Reference<Window> window;
std::array<std::unique_ptr<ModelInstance>, 4> models;
inline constexpr int EXTRA_MODEL_COUNT = 512;
std::array<std::unique_ptr<ModelInstance>, EXTRA_MODEL_COUNT> extramodels;
std::array<Engine::Reference<Camera>, 2> cameras;
std::array<Engine::Reference<Light>, 2> lights;
Engine::Reference<World> world;

std::optional<ADFEntry> QueuedLoad;
GameState CurrentGameState = GameState::Normal;

void gameinit() {
	Engine::RegisterDefaultEngineEntityTypes();

	renderer = Renderer::Make("STDGLRenderer");
	rworld = renderer->MakeRWorld();

	world = World::Make("Primary", rworld);

	window = renderer->MakeWindow(800, 600, "Amethyst");
	window->SetUIFunction(mainuifunction);
	window->SetFullscreen(false);
	UIInit();

	auto savefile = ADFEntry::FromFile("saves/testsave.adf");
	world->Load(savefile);
	auto newsavefile = world->Save();
	newsavefile.ToFile("saves/hi.adf");
	newsavefile.ToFile("saves/hicompacted.adf", true);

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


	for (int i = 0; i < EXTRA_MODEL_COUNT; i++) {
		extramodels[i] = (rworld->MakeModelInstance("backpack.amdl"));
	}

	std::cout << "Hello, world!(From game dll!)" << std::endl;

}



static void GameStateHandler_Normal() {
	static float position = 0;
	if (models[0])
		models[0]->SetMatrix(mat4(1, 0, 0, -128));

	float x = 128.0f;
	for (auto& model : extramodels) {
		model->SetMatrix(mat4(1.0f, 0.0f, 0.0f, 0.0f,
		 					  0.0f, 1.0f, 0.0f, -x,
		 					  0.0f, 0.0f, 1.0f, 0.0f,
		 					  0.0f, 0.0f, 0.0f, 1.0f));
		x += 128.0f;
	}

	world->Update();
	world->Draw();

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
	world->Draw();
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