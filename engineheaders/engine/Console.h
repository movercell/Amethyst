#pragma once

#include "engine/entities/Entity.h"

class ConsoleCommand;
#define ConsoleCommandLambda [](World* InWorld, int AsEntityFromSlot, std::vector<std::string> Do)

namespace Engine {

    void ENGINEEXPORT ExecuteConsoleCommand(World* InWorld, int AsEntityFromSlot, std::string Do);

    namespace Internal {
        void ENGINEEXPORT RegisterConsoleCommand(std::string_view Name, ConsoleCommand* Command);
    }
}

/*! \brief Console command, can be used in the console.
*
* WARNING: To be located in static memory only!
*/
class ConsoleCommand {
    std::string_view Name;
    std::string_view Help;
    std::function<void(World*, int, std::vector<std::string>)> Command;

public:
    ConsoleCommand(std::string_view Named, decltype(Command) Function, std::string_view HelpString = "") : Name(Named), Command(std::move(Function)), Help(HelpString) {
        Engine::Internal::RegisterConsoleCommand(Name, this);
    }
    
    void operator()(World* InWorld, int AsEntityFromSlot, std::vector<std::string> Do) { Command(InWorld, AsEntityFromSlot, Do); }
    std::string_view GetName() { return Name; };
    std::string_view GetHelpString() { return Help; };


    // Static memory only.
    void* operator new(size_t) = delete;
    void operator delete(void*) = delete;
    void* operator new[](size_t) = delete;
    void operator delete[](void*) = delete;
    void* operator new(size_t, void*) = delete;
    void* operator new[](size_t, void*) = delete;
};
