#pragma once
#include "engine/master.h"
#include "engine/filesystem/ADF.h"
#include <string>
#include <map>
#include <variant>
#include <utility>
#include <functional>
#include "World.h"

struct iEntHandler : public EntityStorage {
    // Set by the world once the entity is made.
    int slot = -1;
    World* world = nullptr;

    virtual void SetProperty(const std::string& name, ADFEntry property) = 0;
    virtual std::optional<ADFEntry> GetProperty(const std::string& name) = 0;
    virtual void PropertiesFromADF(const ADFEntry& Saved) = 0;
    virtual ADFEntry PropertiesToADF() = 0;

    virtual void InitEntity() = 0;
    virtual void UpdateEntity() = 0;

    virtual const char* GetClassname() const = 0;
    virtual std::optional<iEntHandler*> GetParent() const = 0;

    virtual void AddTag(const std::string& tag) = 0;
    virtual bool HasTag(const std::string& tag) = 0;
    virtual ADFEntry TagsToADF() const = 0;

    virtual ~iEntHandler() {};

    //! Removes the entity from it's slot, resulting in destruction when not owned by anything else.
    virtual void Remove() {
        auto parent = GetParent();
        if (parent) {
            (*parent.value())[slot] = nullptr;
            return;
        }
        (*world)[slot] = nullptr;
    }
};


struct BaseEntity;

template<typename T>
class BaseEntityHandler : public iEntHandler {
    // This was gotten from the internet
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
protected:

    using EntPropertyLocation = std::variant<int T::*, float T::*, bool T::*, vec2 T::*, vec3 T::*, vec4 T::*, quat T::*, std::string T::*>;

    const char* classname;
    const std::optional<iEntHandler*> parent;
    std::vector<std::string> tags;

    static inline std::map<std::string, EntPropertyLocation> Properties;

    static inline void AddProperty(std::string name, EntPropertyLocation property) { Properties.emplace(name, property); }

    inline ADFEntry PropertyToADF(const EntPropertyLocation Property) {
        return std::visit<ADFEntry>(overload {
        [this](int T::* entproperty)         {  return ADFEntry::String(std::to_string(Entity.*entproperty)); },
        [this](float T::* entproperty)       {  return ADFEntry::String(std::to_string(Entity.*entproperty)); },
        [this](bool T::* entproperty)        {  return ADFEntry::String(Entity.*entproperty ? "1" : "0"); },
        [this](vec2 T::* entproperty)        {  return ADFEntry::Vector2(Entity.*entproperty); },
        [this](vec3 T::* entproperty)        {  return ADFEntry::Vector3(Entity.*entproperty); },
        [this](vec4 T::* entproperty)        {  return ADFEntry::Vector4(Entity.*entproperty); },
        [this](quat T::* entproperty)        {  return ADFEntry::Quaternion(Entity.*entproperty); },
        [this](std::string T::* entproperty) {  return ADFEntry::String(Entity.*entproperty); }
        }, Property);
    }

public:
    T Entity;
    

    static void PropertyInit() {
        // Public-facing properties.
        AddProperty("targetname",            &T::targetname);
        AddProperty("position",              &T::position);
        AddProperty("angles",                &T::angles);
        AddProperty("scale",                 &T::scale);

        // Internal properties.
        AddProperty("rotation",              &T::rotation);
    }


    void SetProperty(const std::string& Name, ADFEntry Property) {
        try {
            std::visit(overload {
            [this, Property](int T::* entproperty)         { Entity.*entproperty  = std::stoi(Property.GetString()); },
            [this, Property](float T::* entproperty)       { Entity.*entproperty  = std::stof(Property.GetString()); },
            [this, Property](bool T::* entproperty)        { Entity.*entproperty  = Property.GetString() == "1"; },
            [this, Property](vec2 T::* entproperty)        { Entity.*entproperty  = Property.GetVec2(); },
            [this, Property](vec3 T::* entproperty)        { Entity.*entproperty  = Property.GetVec3(); },
            [this, Property](vec4 T::* entproperty)        { Entity.*entproperty  = Property.GetVec4(); },
            [this, Property](quat T::* entproperty)        { Entity.*entproperty  = Property.GetQuat(); },
            [this, Property](std::string T::* entproperty) { Entity.*entproperty  = Property.GetString(); }
        }, Properties.at(Name)); } catch(...) {}
    }

    std::optional<ADFEntry> GetProperty(const std::string& name) {
        try {
            return PropertyToADF(Properties[name]);
        } catch(...) {
            return std::nullopt;
        }
    }

    ADFEntry PropertiesToADF() {
        Entity.OnSave();

        ADFEntry ret = ADFEntry::Map();
        auto& retmap = ret.GetChildren();

        for (auto property : Properties) {
            retmap.emplace(property.first, PropertyToADF(Properties.at(property.first)));
        }

        return ret;
    }



    void InitEntity() { Entity.handler = reinterpret_cast<BaseEntityHandler<BaseEntity>*>(this); Entity.world = world; Entity.Initialize(); }
    void UpdateEntity() { Entity.Update(); }

    const char* GetClassname() const { return classname; }
    std::optional<iEntHandler*> GetParent() const { return parent; }

    inline void AddTag(const std::string& tag) { tags.push_back(tag); }
    inline bool HasTag(const std::string& tag) { return std::find(tags.begin(), tags.end(), tag) != tags.end(); }
    inline ADFEntry TagsToADF() const {
        ADFEntry ret = ADFEntry::Array();
        auto& retarr = ret.GetArray();

        for (const auto& tag : tags) {
            retarr.emplace_back(ADFEntry::String(tag));
        }

        return ret;
    }


    void PropertiesFromADF(const ADFEntry& Saved) {
        const auto& Data = Saved.GetChildren();
        for (const auto& property : Data) {
            SetProperty(property.first, property.second);
        }
    }
    BaseEntityHandler(const char* Classname, World* World, std::optional<iEntHandler*> Parent) : classname(Classname), parent(Parent) { world = World; }
    ~BaseEntityHandler() = default;
};

struct BaseEntity {
    World* world;
    BaseEntityHandler<BaseEntity>* handler;

    std::string targetname;
    vec3 position;
    vec3 angles;
    vec3 scale = vec3(1.0f, 1.0f, 1.0f);

    quat rotation;

    virtual void Initialize() {
        if (!HasTag("WasAlreadyInitializedOnce")) {
            rotation = quat(angles);

            AddTag("WasAlreadyInitializedOnce");
        }
    }
    virtual void Update() {}
    virtual void OnSave() {}

    mat4 MakeTransformationMatrix() {
        mat4 result = mat4(scale.x, 0.0f, 0.0f, 0.0f,
                           0.0f, scale.y, 0.0f, 0.0f,
                           0.0f, 0.0f, scale.z, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f);
        result *= rotation.MakeRotationMatrix();
        result[3] = position;
        return result;
    }

    // Handler wrapper functions
    inline const char* GetClassname() const { return handler->GetClassname(); }
    inline std::optional<iEntHandler*> GetParent() const { return handler->GetParent(); };
    inline void AddTag(const std::string& tag) { handler->AddTag(tag); };
    inline bool HasTag(const std::string& tag) { return handler->HasTag(tag); };
};



namespace Engine {
    namespace Internal {
        void ENGINEEXPORT RegisterEntityCreationLambda(const char* classname, std::function<std::shared_ptr<iEntHandler>(World*, std::optional<iEntHandler*>)> Lambda);
    }
}

template<template <typename> typename Handler, typename Entity>
void RegisterEntityType(const char* classname) {
    Handler<Entity>::PropertyInit();
    Engine::Internal::RegisterEntityCreationLambda(classname, [classname](World* world, std::optional<iEntHandler*> parent) {
        return std::make_shared<Handler<Entity>>(classname, world, parent);
    });
}