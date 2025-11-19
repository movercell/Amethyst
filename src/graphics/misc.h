#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>

namespace GraphicsMisc {
    void windowFocusCallback(GLFWwindow* window, int focused);
    void windowCursorEnterCallback(GLFWwindow* window, int entered);
    void windowCursorPosCallback(GLFWwindow* window, double x, double y);
    void windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void windowKeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    void windowCharCallback(GLFWwindow* window, unsigned int c);

    void windowSetCallbacks(GLFWwindow*);
}
