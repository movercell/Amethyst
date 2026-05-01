#include <ranges>

#include "engine/entities/Entity.h"
#include "engine/entities/World.h"

#define WORLD_RESIZE_ADDITIONAL_SLOT_AMOUNT 4096


std::shared_ptr<iEntHandler> World::MakeEntity(std::string classname) {

    std::shared_ptr<iEntHandler> Entity = static_cast<std::shared_ptr<iEntHandler>>(std::make_shared<BaseEntityHandler<BaseEntity>>("info_target"));
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
    EntityHandlers[index] = std::move(Entity);

    return Entity;
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