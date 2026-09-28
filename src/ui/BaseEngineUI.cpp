#include "BaseEngineUI.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "engine/Console.h"

ConsoleVariable<bool> DemoWindowOpen("engine_ui_showdemo", false, false, "Shows the Dear ImGUI demo window.");

void Engine::DrawEngineUI() {
    Engine::Internal::DrawConsole();

    if (DemoWindowOpen) {
        ImGui::ShowDemoWindow(&DemoWindowOpen.GetValue());
    }
}
