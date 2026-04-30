#pragma once

#include <memory>
#include <vector>
#include <deque>
#include "engine/filesystem/ADF.h"

struct iEntHandler;


class World {
    std::vector<std::unique_ptr<iEntHandler>> EntityHandlers;
    std::deque<int> FreedIndices;
    int NextIndexToMake = 0;
    int EntityCount = 0;

public:
    ADFEntry ToADF();
    void FromADF(ADFEntry Saved);

    void AddEntity(std::unique_ptr<iEntHandler> Entity);
    void AddEntityToSlot(std::unique_ptr<iEntHandler> Entity, int Slot);
    void RemoveEntityInSlot(int Slot);
    void TakeEntityFromSlot(int Slot);

    void Update();
};