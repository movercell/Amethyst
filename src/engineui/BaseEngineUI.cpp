#include "BaseEngineUI.h"
#include "imgui.h"
#include "engine/Console.h"

ConsoleVariable<bool> DemoWindowOpen = {"ui_showdemo", false, false, "Shows the Dear ImGUI demo window."};
ConsoleVariable<bool> ConsoleWindowOpen = {"ui_showconsole", false, false, "Shows the console."};

void Engine::DrawEngineUI() {
    DrawConsole();

    if (DemoWindowOpen) {
        ImGui::ShowDemoWindow(&DemoWindowOpen.GetValue());
    }
}


void Engine::DrawConsole() {
    
}