#include "Init.h"
#include "graphics/opengl/STDGLRenderer.h"
#include "engine/entities/Entity.h"

static void exitfunc() {
	glfwTerminate();
}

void Engine::Init() {
    glfwInit();
	std::atexit(&exitfunc);

    Renderer::AddRenderer("STDGLRenderer", &STDGLRenderer::Make);

    RegisterEntityType<BaseEntityHandler, BaseEntity>("info_target");
}