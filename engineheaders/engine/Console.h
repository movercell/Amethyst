#pragma once

#include "engine/entities/Entity.h"

namespace Engine {
    class ConsoleCommand;

    void ENGINEEXPORT ExecuteConsoleCommand(Engine::Reference<World> InWorld, int AsEntityFromSlot, std::string Do);

    namespace Internal {
        void ENGINEEXPORT RegisterConsoleCommand(std::string Name, ConsoleCommand* Command);
    }

    /*! \brief Console command, can be used in the console.
    *
    * WARNING: To be located in static memory only!
    */
    class ConsoleCommand {
        std::function<void(Engine::Reference<World>, int, std::vector<std::string>)> Command;

    public:
        ConsoleCommand(std::string Name, decltype(Command) Function) : Command(Function) {
        #ifndef AMETHYSTENGINESRC
            Engine::Internal::RegisterConsoleCommand(std::move(Name), this);
        #endif
        }
        
        void operator()(Engine::Reference<World> InWorld, int AsEntityFromSlot, std::vector<std::string> Do) { Command(InWorld, AsEntityFromSlot, Do); }


        // Static memory only.
        void* operator new(size_t) = delete;
        void operator delete(void*) = delete;
        void* operator new[](size_t) = delete;
        void operator delete[](void*) = delete;
        void* operator new(size_t, void*) = delete;
        void* operator new[](size_t, void*) = delete;
    };
}