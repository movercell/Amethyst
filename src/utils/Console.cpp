#include "engine/Console.h"
#include "engine/StringUtils.h"


// To avoid initialization order fiasco when creating a console command in the engine itself.
// (Yes, those leaks are intentional.(Because at least one object that needs one of these would be created prior to them and make a std::at_exit screw up if they were actual static objects.))
auto& GetCommandMap() {
    static auto& commands = *(new std::map<std::string_view, ConsoleCommand*>);
    return commands;
}
auto& GetConsoleVariablePreservationLambdasVector() {
    static auto& preservationlambdas = *(new std::vector<std::function<std::string()>>);
    return preservationlambdas;
}
auto& GetWorldMap() {
    static auto& worlds = *(new std::map<std::string_view, World*>);
    return worlds;
}


void Engine::Internal::RegisterConsoleCommand(std::string_view Name, ConsoleCommand* Command) {
    if (GetWorldMap().contains(Name)) {
        Engine::Error("Attempted to create a console command of the same name as another!");
    }
    
    GetCommandMap().emplace(Name, Command);
}
void Engine::Internal::RegisterConsoleVariablePreservation(std::function<std::string()> Function) {
    GetConsoleVariablePreservationLambdasVector().emplace_back(Function);
}
void Engine::Internal::RegisterWorldForConsole(std::string_view Name, World* world) {
    if (GetWorldMap().contains(Name)) {
        Engine::Error("Attempted to create a world of the same name as another!");
    }

    GetWorldMap().emplace(Name, world);
}
void Engine::Internal::UnregisterWorldForConsole(std::string_view Name) {
    GetWorldMap().erase(Name);
}



#define PUSH_PARAM \
                if (CurrentContent.size() > 0) { \
                    Params.emplace_back(CurrentContent.begin(), CurrentContent.end()); \
                    CurrentContent.clear(); \
                }

static auto CommandParseDo(std::string Do) {
    std::vector<std::vector<std::string>> Commands;
    int i = 0;
    while (i < Do.length()) {
        std::vector<std::string> Params;
        std::inplace_vector<char, 1024> CurrentContent;

        while (i < Do.length()) {
            // Quation mark.
            if (Do[i] == '\"') {
                i++;
                while (i < Do.length() && Do[i] != '\"') {
                    // Escape.(Yes this is just copied.)
                    if (Do[i] == '\\') {
                        i++;
                        // Failsafe when last char.
                        if (i == Do.size())
                            break;
                        CurrentContent.push_back(Engine::CharacterEscapeResult(Do[i]));
                        i++;
                        continue;
                    }

                    CurrentContent.push_back(Do[i]);
                    i++;
                }
                PUSH_PARAM;
                i++;
            }

            // Semicolon.
            if (Do[i] == ';') {
                PUSH_PARAM;
                i++;
                break;
            }
            // Escape.
            if (Do[i] == '\\') {
                i++;
                // Failsafe when last char.
                if (i == Do.size())
                    break;
                CurrentContent.push_back(Engine::CharacterEscapeResult(Do[i]));
                i++;
                continue;
            }
            // Whitespace.
            if (!std::isgraph(Do[i])) {
                PUSH_PARAM;
                i++;
                continue;
            }
            
            // Normal letter.
            CurrentContent.push_back(Do[i]);
            i++;
        }
        PUSH_PARAM;

        Commands.emplace_back(std::move(Params));
    }

    return Commands;
}

void Engine::ExecuteConsoleCommand(World* InWorld, int AsEntityFromSlot, std::string Do) {
    if (InWorld == nullptr) {
        Engine::Warning("Ran command in a world passed in as nullptr!");
        return;
    }
    if (!(*InWorld)[AsEntityFromSlot]) {
        Engine::Warning("Ran command as an invalid entity!");
        return;
    }

    auto Commands = CommandParseDo(Do);

    for (auto& Params : Commands) {
        try {
            GetCommandMap().at(Params[0])->operator()(InWorld, AsEntityFromSlot, Params);
        } catch( std::out_of_range e ) {
            Engine::Print(std::string("Unknown console command: " + Params[0]));
        }
    }
}

// Default engine console commands.
ConsoleCommand helpCommand("help", []ConsoleCommandLambda {
    if (Do.size() != 2) {
        Engine::Print("Usage: help [command]");
        return;
    }

    try {
        Engine::Print(std::string(GetCommandMap().at(Do[1])->GetHelpString()));
    } catch( std::out_of_range e ) {
        Engine::Print(Do[1] + " is not a valid console command!");
    }
}, "Returns the help string of a command.");