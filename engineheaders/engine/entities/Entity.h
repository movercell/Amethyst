#pragma once
#include "engine/master.h"
#include "engine/filesystem/ADF.h"
#include <string>
#include <map>
#include <variant>
#include <utility>
#include <functional>
#include "World.h"

struct EntityHandler {
    // Set by the world once the entity is made.
    int slot = -1;
    World* world = nullptr;

    EntityStorage Children;

    virtual void SetProperty(const std::string& name, const ADFEntry& data) = 0;
    virtual std::optional<ADFEntry> GetProperty(const std::string& name) = 0;
    virtual void FromADF(const ADFEntry& Saved) = 0;
    virtual ADFEntry ToADF() = 0;

    //! Used for initializing the entity after manually setting it up if you choose not to load it from ADF. 
    virtual void InitEntity() = 0;
    virtual void UpdateEntity() = 0;

    virtual const char* GetClassname() const = 0;
    virtual std::optional<EntityHandler*> GetParent() const = 0;

    virtual void AddTag(const std::string& tag) = 0;
    virtual bool HasTag(const std::string& tag) = 0;

    //! Sets the entity's position to vec3(NAN, NAN, NAN).
    virtual void Banish() = 0;
    virtual vec3 GetPosition() = 0;
    virtual quat GetRotation() = 0;
    virtual std::string GetName() = 0;
    virtual mat4 GetTransformationMatrix() = 0;

    //! Gets a void* to the entity. This function is quite unsafe, are you certain that whatever you're doing is worth it?
    virtual void* GetEntityPtr() = 0;

    virtual ~EntityHandler() {};

    //! Removes the entity from it's slot, resulting in destruction when not owned by anything else.
    virtual void Remove() {
        auto parent = GetParent();
        if (parent) {
            parent.value()->Children[slot] = Engine::Reference<EntityHandler>();
            return;
        }
        (*world)[slot] = Engine::Reference<EntityHandler>();
    }
};


struct BaseEntity;

namespace Engine { namespace Internal {

template<typename T>
class EntityTemplateHandler : public EntityHandler {
protected:
    const char* classname;
    const std::optional<EntityHandler*> parent;
    std::vector<std::string> tags;
    
    inline ADFEntry TagsToADF() const {
        ADFEntry ret = ADFEntry::Array();
        auto& retarr = ret.GetArray();

        for (const auto& tag : tags) {
            retarr.emplace_back(ADFEntry::String(tag));
        }

        return ret;
    }

public:
    T Entity;
    

    static void RegisterType() {

    }


    // TODO: Make a better implementation of this.
    void SetProperty(const std::string& Name, const ADFEntry& Data) {
        auto tmp = ADFEntry::Map();
        tmp.GetMap().emplace(Name, Data);

        tmp.Deserialize(Entity);
    }

    // TODO: Make a better implementation of this.
    std::optional<ADFEntry> GetProperty(const std::string& Name) {
        Entity.OnSave();
        auto tmp = ADFEntry::Serialize(Entity);

        try {
            return tmp.GetMap().at(Name);
        } catch (const std::out_of_range& e) {
            return std::nullopt;
        }
    }

    ADFEntry ToADF() {
        ADFEntry ret = ADFEntry::Map();
        auto& retmap = ret.GetMap();

        Entity.OnSave();

        retmap.emplace("children", world->EntityStorageToADF(&Children));

        retmap.emplace("tags", TagsToADF());

        retmap.emplace("properties", ADFEntry::Serialize(Entity));

        retmap.emplace("classname", ADFEntry::String(classname));

        return ret;
    }
    void FromADF(const ADFEntry& Saved) {
        const auto& Data = Saved.GetMap();

        Data.at("properties").Deserialize(Entity);

        auto& tagarray = Data.at("tags").GetArray();
        for (const auto& tag : tagarray) {
            AddTag(tag.GetString());
        }

        world->EntityStorageFromADF(Data.at("children"), &Children, this);
        
        InitEntity();
    }



    void InitEntity() {
        Entity.handler = this;
        Entity.world = world;
        Entity.Initialize();
        
        AddTag("WasInit");
    }
    void UpdateEntity() { Entity.Update(); }

    const char* GetClassname() const { return classname; }
    std::optional<EntityHandler*> GetParent() const { return parent; }

    inline void AddTag(const std::string& tag) { tags.push_back(tag); }
    inline bool HasTag(const std::string& tag) { return std::find(tags.begin(), tags.end(), tag) != tags.end(); }

    void Banish() { Entity.position = vec3(NAN, NAN, NAN); }
    vec3 GetPosition() { return Entity.position; }
    quat GetRotation() { return Entity.rotation; }
    std::string GetName() { return Entity.targetname; }
    mat4 GetTransformationMatrix() { return Entity.TransformationMatrix; }

    void* GetEntityPtr() { return &Entity; }

    EntityTemplateHandler(const char* Classname, World* World, std::optional<EntityHandler*> Parent) : classname(Classname), parent(Parent) { world = World; }
    ~EntityTemplateHandler() = default;
};

} /*namespace Internal*/ } /*namespace Engine*/



struct BaseEntity {
    World* world;
    EntityHandler* handler;

    ADFSerialize std::string targetname;
    ADFSerialize vec3 position;
    ADFSerialize vec3 scale = vec3(1.0f, 1.0f, 1.0f);

    ADFSerialize quat rotation;

    mat4 TransformationMatrix;

    virtual void Initialize() {}
    virtual void Update() {
        TransformationMatrix = mat4(scale.x, 0.0f, 0.0f, 0.0f,
                           0.0f, scale.y, 0.0f, 0.0f,
                           0.0f, 0.0f, scale.z, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f);
        TransformationMatrix *= rotation.MakeRotationMatrix();
        TransformationMatrix[3] = position;

        auto parent = handler->GetParent();
        if (parent) {
            TransformationMatrix *= parent.value()->GetTransformationMatrix();
        }
    }
    virtual void OnSave() {}

    // Handler wrapper functions
    inline const char* GetClassname() const { return handler->GetClassname(); }
    inline std::optional<EntityHandler*> GetParent() const { return handler->GetParent(); };
    inline void AddTag(const std::string& tag) { handler->AddTag(tag); };
    inline bool HasTag(const std::string& tag) { return handler->HasTag(tag); };
};



namespace Engine {
    namespace Internal {
        void ENGINEEXPORT RegisterEntityCreationLambda(const char* classname, std::function<Engine::Reference<EntityHandler>(World*, std::optional<EntityHandler*>)> Lambda);

        // Annotation for an entity class
        struct EntityClassnameAnnotation {
            const char* classname;
        };
    }

    template<typename Entity>
    void RegisterEntityClass() {
        static_assert(std::is_base_of_v<BaseEntity, Entity>, "An entity class must be an extension of BaseEntity!");
        constexpr auto EntityClassAnnotations = std::define_static_array(std::meta::annotations_of_with_type(^^Entity, ^^Engine::Internal::EntityClassnameAnnotation));
        static_assert(EntityClassAnnotations.size() == 1, "An entity class must have exactly 1 EntityClassname annotation!");

        constexpr auto EntityClassnameAnnotation = std::meta::extract<Engine::Internal::EntityClassnameAnnotation>(EntityClassAnnotations[0]);
        const char* classname = EntityClassnameAnnotation.classname;

        Engine::Internal::EntityTemplateHandler<Entity>::RegisterType();

        Engine::Internal::RegisterEntityCreationLambda(classname, [classname](World* world, std::optional<EntityHandler*> parent) -> Engine::Reference<EntityHandler> {
            return Engine::Reference(new Engine::UnmanagedInterfacedResource<EntityHandler, Engine::Internal::EntityTemplateHandler<Entity>>(classname, world, parent));
        });
    }

    void ENGINEEXPORT RegisterDefaultEngineEntityTypes();
}

#ifdef __INTELLISENSE__ 
#define EntityClassname(classname) 
#else
#define EntityClassname(classname)  [[= Engine::Internal::EntityClassnameAnnotation{std::define_static_string(classname)} ]]
#endif