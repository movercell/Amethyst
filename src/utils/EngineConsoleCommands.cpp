#include "EngineConsoleCommands.h"
#include "engineui/BaseEngineUI.h"

Engine::ConsoleCommand ui_showdemoCommand("ui_showdemo", ConsoleCommandLambda {
    DemoWindowOpen = !DemoWindowOpen;
});
Engine::ConsoleCommand ui_showconsoleCommand("ui_showconsole", ConsoleCommandLambda {
    ConsoleWindowOpen = !ConsoleWindowOpen;
});

void Engine::RegisterEngineConsoleCommands() {
    Engine::Internal::RegisterConsoleCommand("ui_showdemo", &ui_showdemoCommand);
    Engine::Internal::RegisterConsoleCommand("ui_showconsole", &ui_showconsoleCommand);
}