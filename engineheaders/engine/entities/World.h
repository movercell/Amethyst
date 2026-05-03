#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <algorithm>
#include "engine/filesystem/ADF.h"
#include "engine/master.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"

struct iEntHandler;


class ENGINEEXPORT World {
    std::vector<std::shared_ptr<iEntHandler>> EntityHandlers;
    std::deque<int> FreedIndices;
    int NextIndexToMake = 0;
    int EntityCount = 0;
    std::shared_ptr<RWorld> RenderWorld;

    std::string MapName = "";

    void EntitiesFromADF(const ADFEntry& Saved);

public:
    void Load(const ADFEntry& Saved);
    ADFEntry Save();

    //! Returns an uninitalized entity, or nullptr if classname is not valid.
    std::shared_ptr<iEntHandler> MakeEntity(std::string classname);
    void AddEntityToSlot(std::shared_ptr<iEntHandler> Entity, int Slot);
    std::shared_ptr<iEntHandler> GetEntityInSlot(int Slot);

    inline void RemoveEntityInSlot(int Slot) {
        if (EntityHandlers[Slot]) {
            EntityHandlers[Slot].reset();
            EntityCount--;
        }
    }

    void Update();
    void Clear();

    World(std::shared_ptr<RWorld> Renderworld);
    World(std::shared_ptr<Renderer> Renderer);

    std::shared_ptr<RWorld> GetRWorld() { return RenderWorld; }
};