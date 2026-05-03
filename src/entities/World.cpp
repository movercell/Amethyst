#include <map>
#include <functional>

#include "engine/entities/Entity.h"
#include "engine/entities/World.h"

#define WORLD_RESIZE_ADDITIONAL_SLOT_AMOUNT 4096

static std::map<std::string, std::function<std::shared_ptr<iEntHandler>()>> EntityCreationLambdas;


std::shared_ptr<iEntHandler> World::MakeEntity(std::string classname) {

    std::shared_ptr<iEntHandler> Entity = EntityCreationLambdas[classname]();
    Entity->world = this;

    int index;
    if (FreedIndices.empty()) {
        index = NextIndexToMake++;
    } else {
        index = FreedIndices.front();
        FreedIndices.pop_front();
    }

    EntityCount++;
    if (EntityHandlers.size() < EntityCount) {
        EntityHandlers.resize(EntityHandlers.size() + WORLD_RESIZE_ADDITIONAL_SLOT_AMOUNT);
    }

    Entity->slot = index;
    EntityHandlers[index] = Entity;

    return Entity;
}

void World::AddEntityToSlot(std::shared_ptr<iEntHandler> Entity, int Slot) {
    if (EntityHandlers[Slot]) {
        Engine::Warning("Attempted to add an entity to a slot that's already occupied!(call bumped)");
        return;
    }
    if (Slot >= EntityHandlers.size()) {
        Engine::Warning("Attempted to add an entity into a non-existent slot!(call bumped)");
        return;
    }

        
    auto indexfound = std::find(FreedIndices.begin(), FreedIndices.end(), Slot);
    if (indexfound != FreedIndices.end()) {
        FreedIndices.erase(indexfound);
    }
    
    EntityHandlers[Slot] = Entity;
}



void World::Update() {
    for (auto& Handler : EntityHandlers) {
        if (Handler) Handler->UpdateEntity();
    }
}
void World::Clear() {
    for (auto& Handler : EntityHandlers) {
        Handler.reset();
    }
}



World::World() {
    EntityHandlers.resize(WORLD_RESIZE_ADDITIONAL_SLOT_AMOUNT);
}




void Engine::Internal::RegisterEntityCreationLambda(const char* classname, std::function<std::shared_ptr<iEntHandler>()> Lambda) {
    EntityCreationLambdas.emplace(classname, Lambda);
}