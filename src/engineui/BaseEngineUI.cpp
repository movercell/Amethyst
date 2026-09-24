#include "BaseEngineUI.h"
#include "imgui.h"
#include "engine/Console.h"

bool DemoWindowOpen = false;
bool ConsoleWindowOpen = false;

void Engine::DrawEngineUI() {
    DrawConsole();

    if (DemoWindowOpen) {
        ImGui::ShowDemoWindow(&DemoWindowOpen);
    }
}


void Engine::DrawConsole() {
    
}