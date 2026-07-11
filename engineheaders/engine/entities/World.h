#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <algorithm>
#include "engine/filesystem/ADF.h"
#include "engine/master.h"
#include "engine/graphics/RWorld.h"
#include "engine/graphics/Renderer.h"
#include "engine/Resource.h"

struct iEntHandler;

class EntityStorage {
    using iterator = Engine::Reference<iEntHandler>*;

    static inline constexpr int PreallocatedSlotAmount = 4;
    Engine::Reference<iEntHandler> PreallocatedSlots[PreallocatedSlotAmount];
    Engine::Reference<iEntHandler>* DynamicStorage;
    uint32_t Size = 0;
public:
    void AddEntityBack(Engine::Reference<iEntHandler> Entity);

    int GetFreeIndex();

    void Update();
    void Clear();

    uint32_t size() { return Size; }
    void reserve(uint32_t count);

    iterator begin() {
        if (Size > PreallocatedSlotAmount)
            return DynamicStorage;
        return &(PreallocatedSlots[0]);
    }
    iterator end() {
        if (Size > PreallocatedSlotAmount)
            return DynamicStorage + Size;
        return &(PreallocatedSlots[0]) + Size;
    }
    auto rbegin() { return std::make_reverse_iterator(end()); }
    auto rend() { return std::make_reverse_iterator(begin()); }

    Engine::Reference<iEntHandler>& operator[](uint32_t index) {
        if (!(index < Size)) Engine::Error("Attempted to index an EntityStorage out of bounds!");

        if (Size > PreallocatedSlotAmount)
            return DynamicStorage[index];
        return PreallocatedSlots[index];
    }
    const Engine::Reference<iEntHandler>& operator[](uint32_t index) const {
        if (!(index < Size)) Engine::Error("Attempted to index an EntityStorage out of bounds!");
        
        if (Size > PreallocatedSlotAmount)
            return DynamicStorage[index];
        return PreallocatedSlots[index];
    }
};


class ENGINEEXPORT World : public EntityStorage {
    Engine::Reference<RWorld> RenderWorld;
    std::string MapName = "";

public:
    void Restore(const ADFEntry& Saved);
    ADFEntry Save();

    //! Returns an uninitalized entity, or nullptr if classname is not valid.
    Engine::Reference<iEntHandler> MakeEntity(std::string classname, std::optional<iEntHandler*> parent = std::nullopt);

    void Clear();

    World(Engine::Reference<RWorld> Renderworld);
    World(Engine::Reference<Renderer> Renderer);

    Engine::Reference<RWorld> GetRWorld() { return RenderWorld; }

    void EntityStorageFromADF(const ADFEntry& Saved, EntityStorage* Storage, std::optional<iEntHandler*> parent = std::nullopt);
    ADFEntry EntityStorageToADF(EntityStorage* Storage);
};