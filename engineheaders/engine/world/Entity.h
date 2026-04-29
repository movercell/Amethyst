#pragma once
#include "engine/master.h"
#include "engine/filesystem/ADF.h"
#include <string>
#include <map>
#include <variant>

struct iEntHandler {
    virtual void SetProperty(const std::string& name, ADFEntry property) = 0;
    virtual void InitEntity() = 0;
    virtual ~iEntHandler() {};
    virtual const std::string& GetClassname() = 0;
};

template<typename T>
struct EntProperty {
    using datatype = std::variant<int T::*, float T::*, vec2 T::*, vec3 T::*, vec4 T::*, quat T::*, std::string T::*>;
    datatype data;
    EntProperty(const datatype Data) { data = std::move(Data); };
};

template<typename T>
class BaseEntityHandler : public iEntHandler {
    // This was gotten from the internet
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
protected:

    const std::string& classname;
    static inline std::map<std::string, EntProperty<T>> Properties;

    static inline void AddProperty(std::string name, EntProperty<T> property) { Properties.emplace(name, property); }

public:
    T Entity;
    

    static void PropertyInit() {
        AddProperty("targetname", EntProperty<T>(&T::targetname));
        AddProperty("position",   EntProperty<T>(&T::position));
        AddProperty("rotation",   EntProperty<T>(&T::rotation));
    }


    void SetProperty(const std::string& Name, ADFEntry Property) {
        std::visit(overload {
        [this, Property](int T::* entproperty)         { Entity.*entproperty  = std::stoi(Property.GetString()); },
        [this, Property](float T::* entproperty)       { Entity.*entproperty  = std::stof(Property.GetString()); },
        [this, Property](vec2 T::* entproperty)        { Entity.*entproperty  = Property.GetVec2(); },
        [this, Property](vec3 T::* entproperty)        { Entity.*entproperty  = Property.GetVec3(); },
        [this, Property](vec4 T::* entproperty)        { Entity.*entproperty  = Property.GetVec4(); },
        [this, Property](quat T::* entproperty)        { Entity.*entproperty  = Property.GetQuat(); },
        [this, Property](std::string T::* entproperty) { Entity.*entproperty  = Property.GetString(); }
        }, Properties.at(Name).data);
    }

    void InitEntity() { Entity.Init(); }
    const std::string& GetClassname() { return classname; }


    BaseEntityHandler(const std::pair<std::string, ADFEntry>& Saved) : classname(Saved.first) { 
        const auto& Data = Saved.second.GetChildren();
        for (const auto& property : Data) {
            SetProperty(property.first, property.second);
        }
    }
    ~BaseEntityHandler() = default;
};

struct BaseEntity {
    virtual void Init() {};

    std::string targetname;
    vec3 position;
    quat rotation;
};
