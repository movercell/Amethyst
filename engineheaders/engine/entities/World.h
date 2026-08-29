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

struct EntityHandler;

class ENGINEEXPORT EntityStorage {
    using iterator = Engine::Reference<EntityHandler>*;

    static inline constexpr int PreallocatedSlotAmount = 4;
    Engine::Reference<EntityHandler> PreallocatedSlots[PreallocatedSlotAmount];
    Engine::Reference<EntityHandler>* DynamicStorage;
    uint32_t Size = 0;
public:
    void AddEntityBack(Engine::Reference<EntityHandler> Entity);

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

    Engine::Reference<EntityHandler>& operator[](uint32_t index) {
        if (!(index < Size)) Engine::Error("Attempted to index an EntityStorage out of bounds!");

        if (Size > PreallocatedSlotAmount)
            return DynamicStorage[index];
        return PreallocatedSlots[index];
    }
    const Engine::Reference<EntityHandler>& operator[](uint32_t index) const {
        if (!(index < Size)) Engine::Error("Attempted to index an EntityStorage out of bounds!");
        
        if (Size > PreallocatedSlotAmount)
            return DynamicStorage[index];
        return PreallocatedSlots[index];
    }
};


class ENGINEEXPORT World : public EntityStorage {
    Engine::Reference<RWorld> RenderWorld;
    std::string MapName = "";

    World() {};
public:
    void Restore(const ADFEntry& Saved);
    ADFEntry Save();

    //! Returns an uninitalized entity, or nullptr if classname is not valid.
    Engine::Reference<EntityHandler> MakeEntity(std::string classname, std::optional<EntityHandler*> parent = std::nullopt);

    void Clear();

    static Engine::Reference<World> Make(Engine::Reference<RWorld> Renderworld);
    static Engine::Reference<World> Make(Engine::Reference<Renderer> Renderer);

    Engine::Reference<RWorld> GetRWorld() { return RenderWorld; }

    void EntityStorageFromADF(const ADFEntry& Saved, EntityStorage* Storage, std::optional<EntityHandler*> parent = std::nullopt);
    ADFEntry EntityStorageToADF(EntityStorage* Storage);
};