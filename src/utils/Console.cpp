#include "engine/Console.h"


static std::map<std::string, Engine::ConsoleCommand*> commands;

void Engine::Internal::RegisterConsoleCommand(std::string Name, ConsoleCommand* Command) {
    commands.emplace(Name, Command);
}

void Engine::ExecuteConsoleCommand(World* InWorld, BaseEntity* AsEntity, std::vector<std::string> Do) {

}