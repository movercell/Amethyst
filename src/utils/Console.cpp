#include "engine/Console.h"
#include "engine/StringUtils.h"


static std::map<std::string, Engine::ConsoleCommand*> commands;

void Engine::Internal::RegisterConsoleCommand(std::string Name, ConsoleCommand* Command) {
    commands.emplace(Name, Command);
}

#define PUSH_PARAM \
                if (CurrentContent.size() > 0) { \
                    params.emplace_back(CurrentContent.begin(), CurrentContent.end()); \
                    CurrentContent.clear(); \
                }

void Engine::ExecuteConsoleCommand(Engine::Reference<World> InWorld, int AsEntityFromSlot, std::string Do) {
    int i = 0;
    while (i < Do.length()) {
        std::vector<std::string> params;
        std::inplace_vector<char, 1024> CurrentContent;

        while (i < Do.length()) {
            // TODO: "

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
            commands.at(params[0])->operator()(InWorld, AsEntityFromSlot, params);
        } catch( std::out_of_range e ) {
            Engine::Print(std::string("Unknown console command: " + params[0]));
        }
    }
}