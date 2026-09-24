#include "Init.h"
#include "graphics/opengl/STDGLRenderer.h"
#include "engine/entities/Entity.h"
#include "entities/BasicEntities.h"
#include "entities/MiscEntities.h"
#include "utils/EngineConsoleCommands.h"

static void exitfunc() {
	glfwTerminate();
}

void Engine::Init() {
    glfwInit();
	std::atexit(&exitfunc); // To make it outlive the game dll internal global objects.

    Engine::RegisterEngineConsoleCommands();

    Renderer::AddRenderer("STDGLRenderer", &STDGLRenderer::Make);
}

void Engine::RegisterDefaultEngineEntityTypes() {
    Engine::RegisterEntityClass<EngineEntities::Target>();
    Engine::RegisterEntityClass<EngineEntities::DynamicProp>();
    Engine::RegisterEntityClass<EngineEntities::LightSpot>();
    Engine::RegisterEntityClass<EngineEntities::PointRotating>();
}