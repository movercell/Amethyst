#pragma once

#include "engine/entities/Entity.h"

namespace Engine {
    class ConsoleCommand;

    void ExecuteConsoleCommand(World* InWorld, BaseEntity* AsEntity, std::vector<std::string> Do);

    namespace Internal {
        void RegisterConsoleCommand(std::string Name, ConsoleCommand* Command);
    }

    /*! \brief Console command, can be used in the console.
    *
    * WARNING: To be located in static memory only!
    */
    class ConsoleCommand {
        std::function<void(World*, BaseEntity*, std::vector<std::string>)> Command;

    public:
        ConsoleCommand(std::string Name, decltype(Command) Function) : Command(Function) {
        #ifndef AMETHYSTENGINESRC
            Engine::Internal::RegisterConsoleCommand(std::move(Name), this);
        #endif
        }
        
        void operator()(World* InWorld, BaseEntity* AsEntity, std::vector<std::string> Do) { Command(InWorld, AsEntity, Do); }


        // Static memory only.
        void* operator new(size_t) = delete;
        void operator delete(void*) = delete;
        void* operator new[](size_t) = delete;
        void operator delete[](void*) = delete;
        void* operator new(size_t, void*) = delete;
        void* operator new[](size_t, void*) = delete;
    };
}