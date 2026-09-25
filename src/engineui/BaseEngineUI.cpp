#include "BaseEngineUI.h"
#include "imgui.h"
#include "engine/Console.h"

bool DemoWindowOpen = false;
bool ConsoleWindowOpen = false;

ConsoleCommand ui_showdemoCommand("ui_showdemo", ConsoleCommandLambda {
    DemoWindowOpen = !DemoWindowOpen;
});
ConsoleCommand ui_showconsoleCommand("ui_showconsole", ConsoleCommandLambda {
    ConsoleWindowOpen = !ConsoleWindowOpen;
});

void Engine::DrawEngineUI() {
    DrawConsole();

    if (DemoWindowOpen) {
        ImGui::ShowDemoWindow(&DemoWindowOpen);
    }
}


void Engine::DrawConsole() {
    
}