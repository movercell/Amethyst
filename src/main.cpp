#include <array>
#include <bit>
#include <GLFW/glfw3.h>
#include "engine/filesystem/Filesystem.h"

#include "Init.h"

#include <iostream>
#include <memory>
#include <cstdint>

#include "engine/master.h"
#include "GameDLLHandler.h"

// Time between current frame and last frame
float ENGINEEXPORT deltaTime = 0.0f;	
float lastFrame = 0.0f;

bool EngineShouldTerminate = false;

namespace Engine {
	void QueueShutdown() {
		EngineShouldTerminate = true;
	}
}

int main() {
	Engine::Init();

	GameDLL game;
	game.init();

	while(!EngineShouldTerminate) { // temp
		float currentFrame = static_cast<float>(glfwGetTime());
        float rawdeltaTime = currentFrame - lastFrame;
		deltaTime = std::lerp(deltaTime, rawdeltaTime, 0.1f);
        lastFrame = currentFrame;

		game.loop();
		
		glfwPollEvents();    
	}

}