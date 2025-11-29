#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "graphics/Camera.h"
#include "graphics/RWorld.h"
#include "graphics/Renderer.h"
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "main.h"
#include "graphics/opengl/STDGLRenderer.h"

#include <iostream>
#include <memory>

#include "master.h"

std::function<void(Renderer*)> mainuifunction = [](Renderer* renderer) {
	Camera* camera = renderer->GetCamera("cam2");

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

	//Draws the camera output
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("main", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoBackground);
		std::cout << camera->GetTexture() << std::endl;
		ImGui::Image(camera->GetTexture(), viewport->Size);
	ImGui::End();
	ImGui::PopStyleVar(3);

	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		exit(0);
	}
	ImGui::Begin("Hello from ui function");
		ImGui::Text("hiiiiiiiiii");
	ImGui::End();
	ImGui::ShowDemoWindow();
};

int main() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
	ImGuiContext* uidata = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//Model model = Model("Untitled2.glb");

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	std::shared_ptr openglcontext = STDGLRenderer::Make();
	std::shared_ptr<Window> enginewindow = openglcontext->MakeWindow();
	enginewindow->Update();
	enginewindow->SetUIFunction(mainuifunction);
	RWorld* rworld = openglcontext->newRWorld();
	std::array<std::shared_ptr<Camera>, 2> cameras;
	cameras[0] = rworld->MakeCamera(vec2(800, 600), "cam1");
	cameras[1] = rworld->MakeCamera(vec2(800, 600), "cam2", vec3(1, 1, 1));

	std::cout << "Hello, world!" << std::endl;

	bool isWindowOpen = true;
	while(!glfwWindowShouldClose(window)) {
		openglcontext->Draw();
		glfwMakeContextCurrent(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Start the Dear ImGui frame
		ImGui::SetCurrentContext(uidata);
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
