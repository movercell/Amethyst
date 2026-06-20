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

class EntityStorage : public std::vector<Engine::Reference<iEntHandler>> {
    static inline constexpr int ResizeAdditionalSlotAmount = 16;
public:
    void AddEntityBack(Engine::Reference<iEntHandler> Entity);

    int GetFreeIndex();

    void Update();
    void Clear();
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