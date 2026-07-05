#include <map>
#include <functional>
#include <format>

#include "engine/entities/Entity.h"
#include "engine/entities/World.h"

inline constexpr int SAVEFILE_VERSION = 0;
inline constexpr int WORLD_DEFAULT_SLOT_AMOUNT = 4096;

static std::map<std::string, std::function<Engine::Reference<iEntHandler>(World*, std::optional<iEntHandler*>)>> EntityCreationLambdas;

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
void World::EntityStorageFromADF(const ADFEntry& Saved, EntityStorage* Storage, std::optional<iEntHandler*> parent) {
    const auto& entmap = Saved.GetMap();

    // This line is needed due to the .rbegin() later, otherwise it segfaults when there's no entities in the storage.
    if (entmap.size() == 0) return;

    Storage->reserve(std::stoi(entmap.rbegin()->first) + 1);

    int IndexValidation = -1;
    for (const auto& SavedEntity : entmap) {
        Engine::Reference<iEntHandler> Handler;
        
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
void World::Restore(const ADFEntry& Saved) {
    const auto& Savefile = Saved["Savefile"];
    
    int Saveversion = std::stoi(Savefile["SavefileVersion"].GetString());
    if (Saveversion > SAVEFILE_VERSION) {
        Engine::Warning("Cannot load Savefile: Too old of an engine version.(You should update the game. By the way, for what reason are you trying to load a Savefile in an older version?)");
        return;
    }
    if (Saveversion < SAVEFILE_VERSION) {
        Engine::Error("Savefile updating is not yet supported!");
    }
    

    if (Savefile.HasChild("Mapname")) {
        MapName = Savefile["Mapname"].GetString();
        // TODO: add map file loading here
    }
    EntityStorageFromADF(Savefile["Entities"], this);
}




Engine::Reference<iEntHandler> World::MakeEntity(std::string classname, std::optional<iEntHandler*> parent) {

    Engine::Reference<iEntHandler> Handler;

    try {
        Handler = EntityCreationLambdas.at(classname)(this, parent);
    } catch(const std::out_of_range& e) {
        return nullptr;
    }

    int index;
    if (parent) {
        index = parent.value()->Children.GetFreeIndex();
        parent.value()->Children[index] = Handler;
    } else {
        index = GetFreeIndex();
        (*this)[index] = Handler;
    }

    Handler->slot = index;

    return Handler;
}


void World::Clear() {
    EntityStorage::Clear();
    MapName = "";
}



World::World(Engine::Reference<RWorld> Renderworld) : RenderWorld(Renderworld) {
    reserve(WORLD_DEFAULT_SLOT_AMOUNT);
}
World::World(Engine::Reference<Renderer> Renderer) : RenderWorld(Renderer->MakeRWorld()) {
    reserve(WORLD_DEFAULT_SLOT_AMOUNT);
}



void Engine::Internal::RegisterEntityCreationLambda(const char* classname, std::function<Engine::Reference<iEntHandler>(World*, std::optional<iEntHandler*>)> Lambda) {
    EntityCreationLambdas.emplace(classname, Lambda);
}




void EntityStorage::AddEntityBack(Engine::Reference<iEntHandler> Entity) {
    (*this)[Entity->slot] = Entity;
}

int EntityStorage::GetFreeIndex() {
    auto iterator = std::find(begin(), end(), Engine::Reference<iEntHandler>());
    int ret = iterator - begin(); // Yes this works even when not enough space, since end is one after the last element. 

    if (iterator == end()) {
        reserve(size() + size() / 2);
    }

    return ret;
}

void EntityStorage::reserve(uint count) {
    if (count > Size) {
        if(count > PreallocatedSlotAmount) {
            Engine::Reference<iEntHandler>* OldStorage = begin();
            DynamicStorage = new Engine::Reference<iEntHandler>[count];
                
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
        Handler = Engine::Reference<iEntHandler>();
    }
}