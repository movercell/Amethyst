#pragma once

#include "engine/entities/Entity.h"

class ConsoleCommand;
#define ConsoleCommandLambda (World* InWorld, int AsEntityFromSlot, std::vector<std::string> Do)

namespace Engine {

    void ENGINEEXPORT ExecuteConsoleCommand(World* InWorld, int AsEntityFromSlot, std::string Do);

    namespace Internal {
        void ENGINEEXPORT RegisterConsoleCommand(std::string_view Name, ConsoleCommand* Command);
        void ENGINEEXPORT RegisterConsoleVariablePreservation(std::function<std::string()> Function);
        void RegisterWorldForConsole(std::string_view Name, World* world);
        void UnregisterWorldForConsole(std::string_view Name);
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


/*! \brief Console variable, can be used in the console.
*
* WARNING: To be located in static memory only!
*/
template <typename T>
class ConsoleVariable {
    ConsoleCommand Internalcommand;
    T Value;
    T Min;
    T Max;
    bool Preserved;

public:
    ConsoleVariable(std::string_view Named, T DefaultValue, bool Preservation = false, std::string_view HelpString = "", T MinValue = std::numeric_limits<T>::min(), T MaxValue = std::numeric_limits<T>::max())
    : Internalcommand(Named, [this]ConsoleCommandLambda {
        if constexpr (^^T == ^^bool) {
            if (Do.size() == 1) {
                Value = !Value;
            } else if (Do.size() == 2) {
                Value = Do[1] == "1" ? true : false;
            } else {
                Engine::Print("Usage: " + std::string(Internalcommand.GetName()) + " [Optional: 0 ? 1]");
            }
        } else {
            if (Do.size() == 1) {
                Engine::Print("Current value of " + std::string(Internalcommand.GetName()) + " is: " + std::to_string(Value));
            } else if (Do.size() == 2) {
                std::from_chars(Do[1].data(), Do[1].data() + Do[1].size(), Value);
                Value = std::clamp(Value, Min, Max);
            } else {
                Engine::Print("Usage: " + std::string(Internalcommand.GetName()) + " [Optional: Value]");
            }
        }
    }, HelpString),
    Value(DefaultValue),
    Min(MinValue),
    Max(MaxValue),
    Preserved(Preservation)
    {
        if (Preserved) {
            Engine::Internal::RegisterConsoleVariablePreservation([this](){
                return Internalcommand.GetName() + std::to_string(Value);
            });
        }
    }

    operator T() { return Value; }
    T& GetValue() { return Value; }


    // Static memory only.
    void* operator new(size_t) = delete;
    void operator delete(void*) = delete;
    void* operator new[](size_t) = delete;
    void operator delete[](void*) = delete;
    void* operator new(size_t, void*) = delete;
    void* operator new[](size_t, void*) = delete;
};
