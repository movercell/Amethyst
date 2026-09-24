#include "engine/Console.h"


static std::map<std::string, Engine::ConsoleCommand*> commands;

void Engine::Internal::RegisterConsoleCommand(std::string Name, ConsoleCommand* Command) {
    commands.emplace(Name, Command);
}

void Engine::ExecuteConsoleCommand(Engine::Reference<World> InWorld, int AsEntityFromSlot, std::string Do) {
    Engine::Print("Concommand " + Do);
}