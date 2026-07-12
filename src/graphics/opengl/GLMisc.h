#pragma once

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>

#ifdef DEBUG
    #define GL_PUSH_DEBUG(str) glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, str)
    #define GL_POP_DEBUG glPopDebugGroup()
#else
    #define GL_PUSH_DEBUG(str)
    #define GL_POP_DEBUG
#endif

namespace GLMisc {
    void EnsureGLLoaded();
    void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

    void windowFocusCallback(GLFWwindow* window, int focused);
    void windowCursorEnterCallback(GLFWwindow* window, int entered);
    void windowCursorPosCallback(GLFWwindow* window, double x, double y);
    void windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void windowKeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    void windowCharCallback(GLFWwindow* window, unsigned int c);

    void windowSetCallbacks(GLFWwindow*);
};
