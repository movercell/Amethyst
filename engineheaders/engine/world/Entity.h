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
};

template<typename T>
struct EntProperty {
    using datatype = std::variant<int T::*, float T::*, vec2 T::*, vec3 T::*, vec4 T::*, quat T::*, std::string T::*>;
    datatype data;
    EntProperty(const datatype Data) { data = std::move(Data); };
};

template<typename T>
class BaseEntityHandler : public iEntHandler {
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
public: //temp
    static inline std::map<std::string, EntProperty<T>> Properties;
public:
    T Entity;
    static inline void AddProperty(std::string name, EntProperty<T> property) { Properties.emplace(name, property); }

    static void PropertyInit() {
        AddProperty("targetname", EntProperty<T>(&T::targetname));
        AddProperty("position",   EntProperty<T>(&T::position));
        AddProperty("rotation",   EntProperty<T>(&T::rotation));
        AddProperty("hi",   EntProperty<T>(&T::hi));
    }



    void SetProperty(const std::string& Name, ADFEntry Property) {
        std::visit(overload {
        [this, Property](int T::* entproperty)         { Entity.*entproperty  = std::stoi(Property.GetString()); },
        [this, Property](float T::* entproperty)       { Entity.*entproperty  = std::stof(Property.GetString()); },
        [this, Property](vec2 T::* entproperty)        {  },
        [this, Property](vec3 T::* entproperty)        {  },
        [this, Property](vec4 T::* entproperty)        {  },
        [this, Property](quat T::* entproperty)        {  },
        [this, Property](std::string T::* entproperty) {  }
        }, Properties.at(Name).data);
    }

    void InitEntity() { Entity.Init(); }

    ~BaseEntityHandler() = default;
};

struct BaseEntity {
    virtual void Init() {};

    std::string targetname;
    vec3 position;
    quat rotation;
    int hi;
};
