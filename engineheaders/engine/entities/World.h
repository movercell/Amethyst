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
    void AddEntityToSlot(std::shared_ptr<iEntHandler> Entity, int Slot);
    
    inline void RemoveEntityInSlot(int Slot) {
        if (EntityHandlers[Slot]) {
            EntityHandlers[Slot].reset();
        }
    }

    void Update();
    void Clear();

    World();
};