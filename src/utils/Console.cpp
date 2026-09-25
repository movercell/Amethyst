#include "engine/Console.h"
#include "engine/StringUtils.h"


// To avoid initialization order fiasco when creating a console command in the engine itself.
static inline auto& GetCommandMap() {
    static std::map<std::string_view, ConsoleCommand*> commands;
    return commands;
}

void Engine::Internal::RegisterConsoleCommand(std::string_view Name, ConsoleCommand* Command) {
    GetCommandMap().emplace(Name, Command);
}

#define PUSH_PARAM \
                if (CurrentContent.size() > 0) { \
                    params.emplace_back(CurrentContent.begin(), CurrentContent.end()); \
                    CurrentContent.clear(); \
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

    int i = 0;
    while (i < Do.length()) {
        std::vector<std::string> params;
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

        try {
            GetCommandMap().at(params[0])->operator()(InWorld, AsEntityFromSlot, params);
        } catch( std::out_of_range e ) {
            Engine::Print(std::string("Unknown console command: " + params[0]));
        }
    }
}

// Default engine console commands.
ConsoleCommand helpCommand("help", ConsoleCommandLambda {
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