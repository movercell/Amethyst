#pragma once
#include "engine/master.h"
#include "engine/filesystem/ADF.h"
#include <string>
#include <map>
#include <variant>
#include <utility>
#include <functional>
#include "World.h"

struct iEntHandler {
    // Set by the world once the entity is made.
    int slot = -1;
    World* world = nullptr;

    virtual void SetProperty(const std::string& name, ADFEntry property) = 0;
    virtual std::optional<ADFEntry> GetProperty(const std::string& name) = 0;
    virtual void FromADF(const ADFEntry& Saved) = 0;
    virtual ADFEntry ToADF() = 0;
    virtual void InitEntity() = 0;
    virtual void UpdateEntity() = 0;
    virtual ~iEntHandler() {};
    virtual const char* GetClassname() = 0;

    //! Removes the entity from the world, resulting in destruction when not owned by anything else.
    virtual void Remove() {
        world->RemoveEntityInSlot(slot);
    }
};


struct BaseEntity;

template<typename T>
class BaseEntityHandler : public iEntHandler {
    // This was gotten from the internet
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
protected:

    using EntPropertyLocation = std::variant<int T::*, float T::*, vec2 T::*, vec3 T::*, vec4 T::*, quat T::*, std::string T::*>;

    const char* classname;
    static inline std::map<std::string, EntPropertyLocation> Properties;

    static inline void AddProperty(std::string name, EntPropertyLocation property) { Properties.emplace(name, property); }

    inline ADFEntry PropertyToADF(const EntPropertyLocation Property) {
        return std::visit<ADFEntry>(overload {
        [this](int T::* entproperty)         {  return ADFEntry::String(std::to_string(Entity.*entproperty)); },
        [this](float T::* entproperty)       {  return ADFEntry::String(std::to_string(Entity.*entproperty)); },
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
        AddProperty("targetname", &T::targetname);
        AddProperty("position",   &T::position);
        AddProperty("rotation",   &T::rotation);
    }


    void SetProperty(const std::string& Name, ADFEntry Property) {
        try {
            std::visit(overload {
            [this, Property](int T::* entproperty)         { Entity.*entproperty  = std::stoi(Property.GetString()); },
            [this, Property](float T::* entproperty)       { Entity.*entproperty  = std::stof(Property.GetString()); },
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

    ADFEntry ToADF() {
        ADFEntry ret = ADFEntry::Map();
        auto& retmap = ret.GetChildren();

        retmap.emplace("classname", ADFEntry::String(classname));

        auto& propertymap = retmap.emplace("properties", ADFEntry::Map()).first->second.GetChildren();

        for (auto property : Properties) {
            propertymap.emplace(property.first, PropertyToADF(Properties.at(property.first)));
        }

        return ret;
    }



    void InitEntity() { Entity.handler = reinterpret_cast<BaseEntityHandler<BaseEntity>*>(this); Entity.world = world; Entity.Init(); }
    void UpdateEntity() { Entity.Update(); }
    const char* GetClassname() { return classname; }


    void FromADF(const ADFEntry& Saved) {
        const auto& Data = Saved.GetChildren();
        for (const auto& property : Data) {
            SetProperty(property.first, property.second);
        }
    }
    BaseEntityHandler(const char* Classname, World* World) : classname(Classname) { world = World; }
    ~BaseEntityHandler() = default;
};

struct BaseEntity {
    World* world;
    BaseEntityHandler<BaseEntity>* handler;

    std::string targetname;
    vec3 position;
    quat rotation;

    virtual void Init() {};
    virtual void Update() {};
};



namespace Engine {
    namespace Internal {
        void ENGINEEXPORT RegisterEntityCreationLambda(const char* classname, std::function<std::shared_ptr<iEntHandler>(World*)> Lambda);
    }
}

template<template <typename> typename Handler, typename Entity>
void RegisterEntityType(const char* classname) {
    Handler<Entity>::PropertyInit();
    Engine::Internal::RegisterEntityCreationLambda(classname, [classname](World* world) {
        return std::make_shared<Handler<Entity>>(classname, world);
    });
}