#include "engine/graphics/Renderer.h"
#include <vector>
#include "engine/Console.h"

ConsoleCommand r_commandCommand("r_command", []ConsoleCommandLambda {
    if (Do.size() == 1) {
        Engine::Print("Usage: r_command [Command to pass to the renderer]\n\nHint: Try `r_command help`.(Note: May not work but it's worth a try.)");
        return;
    }
    InWorld->GetRWorld()->GetRenderer()->RendererCommand(Do);
}, "Passes the command to the renderer.");

static std::vector<std::pair<const std::string, Engine::Reference<Renderer>(*)()>> RendererTypes;

Engine::Reference<Renderer> Renderer::Make(std::string classname) {
    for (const auto& makefunc : RendererTypes) {
        if (makefunc.first == classname) return makefunc.second();
    }
    Engine::Error("Tried to instanciate an unknown Renderer type!");
}

void Renderer::AddRenderer(const std::string classname, Engine::Reference<Renderer> (*makefunc)() ) {
    RendererTypes.emplace_back(classname, makefunc);
}
