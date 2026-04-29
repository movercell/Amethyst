#pragma once
#include "engine/master.h"
#include "engine/filesystem/ADF.h"
#include <string>
#include <map>
#include <variant>

struct iEntHandler {
    virtual void SetProperty(const std::string& name, ADFEntry property) = 0;
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
public: //temp
    static inline std::map<std::string, EntProperty<T>> Properties;
    std::optional<T> Entity;
public:
    static inline void AddProperty(std::string name, EntProperty<T> property) { Properties.emplace(name, property); }



    static void PropertyInit() {
        AddProperty("targetname", EntProperty<T>(&T::targetname));
        AddProperty("position",   EntProperty<T>(&T::position));
        AddProperty("rotation",   EntProperty<T>(&T::rotation));
    }



    void SetProperty(const std::string& name, ADFEntry property) {

    }

    ~BaseEntityHandler() = default;
};

struct BaseEntity {
    std::string targetname;
    vec3 position;
    quat rotation;
};
