#include <imgui.h>
#include <glad/glad.h>
#include "GLMisc.h"
#include "STDGLWindow.h"

#include <iostream>
#include <string>
#include <sstream>
#include <utility>

void GLMisc::EnsureGLLoaded() {
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	auto temp = glfwCreateWindow(1, 1, "a", NULL, NULL);
	glfwMakeContextCurrent(temp);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		Engine::Error("Failed to initialize GLAD");
	}
	glfwMakeContextCurrent(NULL);
	glfwDestroyWindow(temp);
}
void GLMisc::GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

	auto const src_str = [source]() {
		switch (source) {
		    case GL_DEBUG_SOURCE_API: return "API";
		    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		    case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
        std::unreachable();
	}();

	auto const type_str = [type]() {
		switch (type) {
		    case GL_DEBUG_TYPE_ERROR: return "ERROR";
		    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		    case GL_DEBUG_TYPE_MARKER: return "MARKER";
		    case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
        std::unreachable();
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		    case GL_DEBUG_SEVERITY_LOW: return "LOW";
		    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
        std::unreachable();
	}();

    std::stringstream MessageStream;
	MessageStream << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
    
    Engine::Error(MessageStream.str());
}



void GLMisc::windowFocusCallback(GLFWwindow* window, int focused) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    if (focused == GLFW_TRUE) {
        WindowObject->ProcessCursorEating();
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    ImGui_ImplGlfw_WindowFocusCallback(window, focused);
}

void GLMisc::windowCursorEnterCallback(GLFWwindow* window, int entered) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}

void GLMisc::windowCursorPosCallback(GLFWwindow* window, double x, double y) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
}

void GLMisc::windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void GLMisc::windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void GLMisc::windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void GLMisc::windowCharCallback(GLFWwindow* window, unsigned int c) {
    auto* WindowObject = reinterpret_cast<STDGLWindow*>(glfwGetWindowUserPointer(window));
    ImGui::SetCurrentContext(WindowObject->UIData);

    ImGui_ImplGlfw_CharCallback(window, c);
}


void GLMisc::windowSetCallbacks(GLFWwindow* window) {
    glfwSetWindowFocusCallback(window, windowFocusCallback);
    glfwSetCursorEnterCallback(window, windowCursorEnterCallback);
    glfwSetCursorPosCallback(window, windowCursorPosCallback);
    glfwSetMouseButtonCallback(window, windowMouseButtonCallback);
    glfwSetScrollCallback(window, windowScrollCallback);
    glfwSetKeyCallback(window, windowKeyCallback);
    glfwSetCharCallback(window, windowCharCallback);
}