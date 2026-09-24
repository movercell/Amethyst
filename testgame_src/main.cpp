#include <array>
#include <bit>
#include "engine/filesystem/Filesystem.h"

#include <iostream>
#include <memory>
#include <cstdint>

#include "engine/master.h"

#include "main.h"
#include "ui.h"

Engine::Reference<Renderer> renderer;
Engine::Reference<RWorld> rworld;
Engine::Reference<Window> window;
std::array<std::unique_ptr<ModelInstance>, 4> models;
std::vector<std::unique_ptr<ModelInstance>> extramodels;
std::array<Engine::Reference<Camera>, 2> cameras;
std::array<Engine::Reference<Light>, 2> lights;
Engine::Reference<World> world;

std::optional<ADFEntry> QueuedLoad;
GameState CurrentGameState = GameState::Normal;

void gameinit() {
	Engine::RegisterDefaultEngineEntityTypes();
	
	Engine::RegisterEntityClass<Entity_Player>();
	Engine::RegisterEntityClass<Entity_PlayerStart>();
	Engine::RegisterEntityClass<Entity_Player_MainMenu>();
	Engine::RegisterEntityClass<Entity_PlayerStart_MainMenu>();

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
	window->SetFullscreen(true);
	UIInit();
	
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