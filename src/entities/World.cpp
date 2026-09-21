#include <map>
#include <functional>
#include <format>

#include "engine/entities/Entity.h"
#include "engine/entities/World.h"

inline constexpr int SAVEFILE_VERSION = 0;
inline constexpr int WORLD_DEFAULT_SLOT_AMOUNT = 4096;
inline constexpr int WORLD_PRESERVED_SLOT_AMOUNT = 32;

static std::map<std::string_view, std::function<Engine::Reference<EntityHandler>(World*, std::optional<EntityHandler*>)>> EntityCreationLambdas;

ADFEntry World::EntityStorageToADF(EntityStorage* Storage) {
    ADFEntry ret = ADFEntry::Map();
    auto& retmap = ret.GetMap();

    for (int i = 0; i < Storage->size(); i++) {
        auto& Handler = (*Storage)[i];
        if (!Handler) continue;

        // The padding is needed for the std::map constructor to properly order the entities on load.
        retmap.emplace(std::format("{:06}", i), Handler->ToADF());
    }

    return ret;
}
void World::EntityStorageFromADF(const ADFEntry& Saved, EntityStorage* Storage, std::optional<EntityHandler*> parent) {
    const auto& entmap = Saved.GetMap();

    // This line is needed due to the .rbegin() later, otherwise it segfaults when there's no entities in the storage.
    if (entmap.size() == 0) return;

    Storage->reserve(std::stoi(entmap.rbegin()->first) + 1);

    int IndexValidation = -1;
    for (const auto& SavedEntity : entmap) {
        if (SavedEntity.first.length() != 6) {
            Engine::Error("Corrupted Savefile: Entity slot index string is not 6 characters in legth.");
        }

        Engine::Reference<EntityHandler> Handler;
        
        try {
            Handler = EntityCreationLambdas.at(SavedEntity.second["classname"].GetString())(this, parent);
        } catch(const std::out_of_range& e) {
            continue;
        }

        int slot = std::stoi(SavedEntity.first);

        if (slot <= IndexValidation) {
            Engine::Error("Corrupted Savefile: Entities out of order or attempting to share the same slot.");
        }
        IndexValidation = slot;

        (*Storage)[slot] = Handler;
        Handler->slot = slot;
        Handler->FromADF(SavedEntity.second);
    }
}


ADFEntry World::Save() {
    ADFEntry ret = ADFEntry::Map();
    ret.GetMap().emplace("Savefile", ADFEntry::Map());
    auto& savemap = ret["Savefile"].GetMap();

    savemap.emplace("MapName", ADFEntry::String(MapName));
    savemap.emplace("MapVersion", ADFEntry::String("thisvalueisnotyetused"));
    savemap.emplace("SavefileVersion", ADFEntry::String(std::to_string(SAVEFILE_VERSION)));
    savemap.emplace("Entities", EntityStorageToADF(this));

    return ret;
}
void World::LoadImmediate(const ADFEntry& Saved) {
    const auto& Savefile = Saved["Savefile"];
    
    int Saveversion = std::stoi(Savefile["SavefileVersion"].GetString());
    if (Saveversion > SAVEFILE_VERSION) {
        Engine::Warning("Cannot load Savefile: Too old of an engine version.(You should update the game. By the way, for what reason are you trying to load a Savefile in an older version?)");
        return;
    }
    if (Saveversion < SAVEFILE_VERSION) {
        Engine::Error("Savefile updating is not yet supported!");
    }
    
    Clear();
    if (Savefile.HasChild("Mapname")) {
        MapName = Savefile["Mapname"].GetString();
        // TODO: add map file loading here
    }
    EntityStorageFromADF(Savefile["Entities"], this);
}




Engine::Reference<EntityHandler> World::MakeEntity(std::string classname, std::optional<EntityHandler*> parent, std::optional<int> forcedslot) {

    Engine::Reference<EntityHandler> Handler;

    try {
        Handler = EntityCreationLambdas.at(classname)(this, parent);
    } catch(const std::out_of_range& e) {
        return nullptr;
    }

    int index;
    if (parent) {
        index = forcedslot ? forcedslot.value() : parent.value()->Children.GetFreeIndex();
        parent.value()->Children[index] = Handler;
    } else {
        index = forcedslot ? forcedslot.value() : GetFreeIndex();
        (*this)[index] = Handler;
    }

    Handler->slot = index;

    return Handler;
}


void World::Clear() {
    EntityStorage::Clear();
    MapName = "";
}

void World::Update() {
    if (QueuedLoad) {
        LoadImmediate(QueuedLoad.value());
        QueuedLoad.reset();
    }

    EntityStorage::Update();
}

World::World(std::string name) {
    Name = name;
    reserve(WORLD_DEFAULT_SLOT_AMOUNT);

    // Need to preserve the preserved slots.
    PreseserveSlots(WORLD_PRESERVED_SLOT_AMOUNT);
}

Engine::Reference<World> World::Make(std::string name, Engine::Reference<RWorld> Renderworld) {
    auto result = new Engine::UnmanagedResource<World>(World(name));
    result->resource.RenderWorld = Renderworld;
    return result;
}
Engine::Reference<World> World::Make(std::string name, Engine::Reference<Renderer> Renderer) {
    auto result = new Engine::UnmanagedResource<World>(World(name));
    result->resource.RenderWorld = Renderer->MakeRWorld();
    return result;
}



void Engine::Internal::RegisterEntityCreationLambda(std::string_view classname, std::function<Engine::Reference<EntityHandler>(World*, std::optional<EntityHandler*>)> Lambda) {
    EntityCreationLambdas.emplace(classname, Lambda);
}




void EntityStorage::AddEntityBack(Engine::Reference<EntityHandler> Entity) {
    (*this)[Entity->slot] = Entity;
}

int EntityStorage::GetFreeIndex() {
    auto iterator = std::find(begin() + PreservedSlotAmount, end(), Engine::Reference<EntityHandler>());
    int ret = iterator - begin(); // Yes this works even when not enough space, since end is one after the last element. 

    if (iterator == end()) {
        reserve(size() + size() / 2);
    }

    return ret;
}

void EntityStorage::reserve(uint32_t count) {
    if (count > Size) {
        if(count > PreallocatedSlotAmount) {
            Engine::Reference<EntityHandler>* OldStorage = begin();
            DynamicStorage = new Engine::Reference<EntityHandler>[count];
                
            for (int i = 0; i < Size; i++) {
                DynamicStorage[i] = OldStorage[i];
            }
            if (Size > PreallocatedSlotAmount) {
                delete[] OldStorage;
            }
        }
        Size = count;
    }
}
void EntityStorage::Update() {
    for (auto& Handler : (*this)) {
        if (Handler) {
            Handler->UpdateEntity();
            Handler->Children.Update();
        }
    }
}
void EntityStorage::Clear() {
    for (auto& Handler : (*this)) {
        Handler = Engine::Reference<EntityHandler>();
    }
}