#pragma once

#include <optional>
#include "engine/graphics/Renderer.h"
#include "engine/filesystem/ADF.h"
#include "engine/entities/Entity.h"
#include "entities/player.h"

enum class GameState {
	Normal,
	Loading,
	Paused
};

extern std::optional<ADFEntry> QueuedLoad;
extern GameState CurrentGameState;
extern Engine::Reference<Renderer> renderer;
extern Engine::Reference<Window> window;
extern Engine::Reference<World> world;
