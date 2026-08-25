#include "Init.h"
#include "graphics/opengl/STDGLRenderer.h"
#include "engine/entities/Entity.h"
#include "entities/BasicEntities.h"
#include "entities/MiscEntities.h"

static void exitfunc() {
	glfwTerminate();
}

void Engine::Init() {
    glfwInit();
	std::atexit(&exitfunc);

    Renderer::AddRenderer("STDGLRenderer", &STDGLRenderer::Make);
}

void Engine::RegisterDefaultEngineEntityTypes() {
    Engine::RegisterEntityType<EngineEntities::Target>();
    Engine::RegisterEntityType<EngineEntities::DynamicProp>();
    Engine::RegisterEntityType<EngineEntities::LightSpot>();
    Engine::RegisterEntityType<EngineEntities::PointRotating>();
}