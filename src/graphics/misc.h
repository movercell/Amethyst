#pragma once

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>

class GraphicsMisc {
public:
    static void windowFocusCallback(GLFWwindow* window, int focused);
    static void windowCursorEnterCallback(GLFWwindow* window, int entered);
    static void windowCursorPosCallback(GLFWwindow* window, double x, double y);
    static void windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void windowKeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    static void windowCharCallback(GLFWwindow* window, unsigned int c);

    static void windowSetCallbacks(GLFWwindow*);
};
