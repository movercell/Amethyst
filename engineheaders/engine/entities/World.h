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

class EntityStorage {
    static inline constexpr int ResizeAdditionalSlotAmount = 16;

    std::vector<std::shared_ptr<iEntHandler>> EntityHandlers;

public:
    void AddEntityBack(std::shared_ptr<iEntHandler> Entity);

    inline std::shared_ptr<iEntHandler>& operator[](int i) { return EntityHandlers[i]; }
    inline const std::shared_ptr<iEntHandler>& operator[](int i) const { return EntityHandlers[i]; }

    inline auto begin() { return EntityHandlers.begin(); }
    inline auto end() { return EntityHandlers.end(); }
    inline auto rbegin() { return EntityHandlers.rbegin(); }
    inline auto rend() { return EntityHandlers.rend(); }
    inline auto size() { return EntityHandlers.size(); }

    inline void resize(int amount) { EntityHandlers.resize(amount); }
    inline void reserve(int amount) { EntityHandlers.reserve(amount); }

    int GetFreeIndex();

    void Update();
    void Clear();
};


class ENGINEEXPORT World : public EntityStorage {
    std::shared_ptr<RWorld> RenderWorld;
    std::string MapName = "";
    void EntityStorageFromADF(const ADFEntry& Saved, EntityStorage* Storage, std::optional<iEntHandler*> parent = std::nullopt);
    ADFEntry EntityStorageToADF(EntityStorage* Storage);

public:
    void Restore(const ADFEntry& Saved);
    ADFEntry Save();

    //! Returns an uninitalized entity, or nullptr if classname is not valid.
    std::shared_ptr<iEntHandler> MakeEntity(std::string classname, std::optional<iEntHandler*> parent = std::nullopt);

    void Clear();

    World(std::shared_ptr<RWorld> Renderworld);
    World(std::shared_ptr<Renderer> Renderer);

    std::shared_ptr<RWorld> GetRWorld() { return RenderWorld; }
};