#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "main.h"
#include "rendering/opengl/GLContext.h"

#include <iostream>


int main() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	std::shared_ptr openglcontext = GLContext::Make();
	if (window == NULL) {
  		std::cout << "Failed to create GLFW window" << std::endl;
  		glfwTerminate();
  		return -1;
	}
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	std::cout << "Hello, world!" << std::endl;

	bool isWindowOpen = true;
	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(); // Show demo window! :)
		//i don't actually have anything to render yet :(

		if (isWindowOpen) {
			ImGui::Begin("Grid Test", &isWindowOpen);
			ImGui::Button("lol", ImVec2(25.0f, 25.0f));
			ImGui::SameLine();
			ImGui::PushID(1);
			ImGui::Button("lol", ImVec2(25.0f, 25.0f));
			ImGui::PopID();
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();    
	}
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}  
