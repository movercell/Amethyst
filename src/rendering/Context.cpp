#define CONTEXT_CPP
#include "rendering/Context.h"
#include <GLFW/glfw3.h>

//first the internal window stuff
//using WindowData = GLFWwindow;

void Context::InternalWindow::Construct(WindowData* Data) {
    data = Data;
    isValid = true;
}

Context::Context() {
}

Context::~Context() {
}