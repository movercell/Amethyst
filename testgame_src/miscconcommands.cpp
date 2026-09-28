#include "engine/Console.h"
#include "main.h"


ConsoleCommand saveCommand("save", []ConsoleCommandLambda {
    if (Do.size() != 2) {
        Engine::Print("Usage: save [path to savefile]");
        return;
    }
    InWorld->Save().ToFile(std::format("saves/{}.adf", Do[1]), true);
}, "Saves the world into a Savefile.");
ConsoleCommand loadCommand("load", []ConsoleCommandLambda {
    if (Do.size() != 2) {
        Engine::Print("Usage: load [path to savefile]");
        return;
    }
    auto savefile = ADFEntry::FromFile(std::format("saves/{}.adf", Do[1]));
    if (savefile.HasChild("Savefile"))
        InWorld->Load(savefile);
}, "Loads the world from a Savefile.");