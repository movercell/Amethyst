#include <map>
#include <functional>

#include "engine/entities/Entity.h"
#include "engine/entities/World.h"

#define WORLD_DEFAULT_SLOT_AMOUNT 4096

static std::map<std::string, std::function<std::shared_ptr<iEntHandler>(World*, std::optional<iEntHandler*>)>> EntityCreationLambdas;

ADFEntry World::EntityStorageToADF(EntityStorage* Storage) {
    ADFEntry ret = ADFEntry::Map();
    auto& retmap = ret.GetChildren();

    for (int i = 0; i < Storage->size(); i++) {
        auto& Handler = (*Storage)[i];
        if (!Handler) continue;

        auto slotstring = std::to_string(i);
        auto& Saved = retmap.emplace(slotstring, ADFEntry::Map()).first->second.GetChildren();

        Saved.emplace("classname",  ADFEntry::String(Handler->GetClassname()));
        Saved.emplace("properties", Handler->PropertiesToADF());
        Saved.emplace("children",   EntityStorageToADF(Handler.get()));
        Saved.emplace("tags",       Handler->TagsToADF());
    }

    return ret;
}
void World::EntityStorageFromADF(const ADFEntry& Saved, EntityStorage* Storage, std::optional<iEntHandler*> parent) {
    const auto& entmap = Saved.GetChildren();
    Storage->resize(Saved.GetChildren().size());

    for (const auto& SavedEntity : entmap) {
        std::shared_ptr<iEntHandler> Handler;
        
        try {
            Handler = EntityCreationLambdas.at(SavedEntity.second["classname"].GetString())(this, parent);
        } catch(const std::out_of_range& e) {
            continue;
        }

        Handler->PropertiesFromADF(SavedEntity.second["properties"]);
        int slot = std::stoi(SavedEntity.first);
        for (const auto& tag : SavedEntity.second["tags"].GetArray()) { Handler->AddTag(tag.GetString()); }
        (*Storage)[slot] = Handler;
        Handler->slot = slot;
        EntityStorageFromADF(SavedEntity.second["children"], Handler.get(), Handler.get());
        Handler->InitEntity();
    }
}


ADFEntry World::Save() {
    ADFEntry ret = ADFEntry::Map();
    ret.GetChildren().emplace("Savefile", ADFEntry::Map());
    auto& savemap = ret["Savefile"].GetChildren();

    savemap.emplace("MapName", ADFEntry::String(MapName));
    savemap.emplace("Entities", EntityStorageToADF(this));

    return ret;
}
void World::Restore(const ADFEntry& Saved) {
    const auto& Savefile = Saved["Savefile"];
    if (Savefile.HasChild("Mapname")) {
        MapName = Savefile["Mapname"].GetString();
        // TODO: add map file loading here
    }
    EntityStorageFromADF(Savefile["Entities"], this);
}




std::shared_ptr<iEntHandler> World::MakeEntity(std::string classname, std::optional<iEntHandler*> parent) {

    std::shared_ptr<iEntHandler> Handler;

    try {
        Handler = EntityCreationLambdas.at(classname)(this, parent);
    } catch(const std::out_of_range& e) {
        return nullptr;
    }

    int index;
    if (parent) {
        index = parent.value()->GetFreeIndex();
        (*parent.value())[index] = Handler;
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



World::World(std::shared_ptr<RWorld> Renderworld) : RenderWorld(Renderworld) {
    reserve(WORLD_DEFAULT_SLOT_AMOUNT);
}
World::World(std::shared_ptr<Renderer> Renderer) : RenderWorld(Renderer->MakeRWorld()) {
    reserve(WORLD_DEFAULT_SLOT_AMOUNT);
}



void Engine::Internal::RegisterEntityCreationLambda(const char* classname, std::function<std::shared_ptr<iEntHandler>(World*, std::optional<iEntHandler*>)> Lambda) {
    EntityCreationLambdas.emplace(classname, Lambda);
}




void EntityStorage::AddEntityBack(std::shared_ptr<iEntHandler> Entity) {
    EntityHandlers[Entity->slot] = Entity;
}

int EntityStorage::GetFreeIndex() {
    auto iterator = std::find(begin(), end(), nullptr);
    int ret = iterator - begin(); // Yes this works even when not enough space, since end is one after the last element. 

    if (iterator == end()) {
        EntityHandlers.resize(size() + ResizeAdditionalSlotAmount);
    }

    return ret;
}

void EntityStorage::Update() {
    for (auto& Handler : (*this)) {
        if (Handler) Handler->UpdateEntity();
    }
}
void EntityStorage::Clear() {
    for (auto& Handler : (*this)) {
        Handler.reset();
    }
}