#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <algorithm>
#include "engine/filesystem/ADF.h"
#include "engine/master.h"

struct iEntHandler;


class ENGINEEXPORT World {
    std::vector<std::shared_ptr<iEntHandler>> EntityHandlers;
    std::deque<int> FreedIndices;
    int NextIndexToMake = 0;
    int EntityCount = 0;

public:
    ADFEntry ToADF();
    void FromADF(ADFEntry Saved);

    // Returns an uninitalized entity.
    std::shared_ptr<iEntHandler> MakeEntity(std::string classname);
    inline void RemoveEntityInSlot(int Slot) {
        if (EntityHandlers[Slot]) {
            EntityHandlers[Slot].reset();
        }
    }
    inline void AddEntityToSlot(std::shared_ptr<iEntHandler> Entity, int Slot) {
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
        
    }

    void Update();
    void Clear();

    World();
};