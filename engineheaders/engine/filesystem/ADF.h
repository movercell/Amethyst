#pragma once
#include "engine/filesystem/Filesystem.h"
#include "engine/master.h"
#include <variant>
#include <optional>
#include <map>
#include <string>
#include <vector>
#include <memory>
/*!
*  \brief Runtime version of an entry from an Amethyst Data Format(.adf) file.
*
*   An entry can be of 3 different types:
*   - A map
*   - An array
*   - A string
*
*   Four compoud (pseudo-)types exist as well:
*   - A 2-component vector
*   - A 3-component vector
*   - A 4-component vector
*   - A quaternion
*
*   These types are not real types and are implemented as map-type entries containing a string-type entry per each component, hovewer they have explicit support by the API and should be used when an entry of such type is required to be stored.  
*
*   Due to how annoying it could be to hunt down malformed/wrongly interpreted ADF trees the engine will raise an Engine::Error when attempting to read an entry as a different type than it is and will also show which file the error came from.
*/
class ADFEntry {
    enum class ADFType {
        map,
        array
    };
    enum class TokenType {
        String,
        StartMap,
        StartArray,
        EndMap,
        EndArray,
        EndFile
    };
    struct Token {
        TokenType type;
        std::optional<std::string> content;
        inline Token() {}
        inline Token (TokenType Type, std::optional<std::string> Content = std::nullopt) { type = std::move(Type); content = std::move(Content); }
    };

    std::variant<std::map<std::string, ADFEntry>, std::string, std::vector<ADFEntry>> data;
    // This is used for showing which file an error came from
    std::shared_ptr<std::string> Filename = nullptr;


    class Tokenizer {
        std::ifstream filestream;
        const std::string& filepath;
    public:
        inline Tokenizer(const std::string& FilePath) : filepath(FilePath) {
            filestream = Filesystem::GetFile(FilePath, std::ios::in | std::ios_base::binary);
        }
        Token ReadToken();
    };

    [[noreturn]] void ADFError(const std::string& error) const;

    ADFEntry(ADFType Type, Tokenizer& Tokenizer, std::shared_ptr<std::string> filename);
    ADFEntry(std::string content, std::shared_ptr<std::string> filename) { data = std::move(content); filename = filename; }
    ADFEntry() {};
public:
    //! Creates an ADF tree from a .adf file.
    static ENGINEEXPORT ADFEntry FromFile(const std::string& FilePath);

    //! Used for manual creation of string-type entries.
    inline static ADFEntry String(std::string Content = std::string()) { ADFEntry ret; ret.data = Content; return ret; }
    //! Used for manual creation of map-type entries.
    inline static ADFEntry Map(std::map<std::string, ADFEntry> Content = std::map<std::string, ADFEntry>()) { ADFEntry ret; ret.data = Content; return ret; }
    //! Used for manual creation of array-type entries.
    inline static ADFEntry Array(std::vector<ADFEntry> Content = std::vector<ADFEntry>()) { ADFEntry ret; ret.data = Content; return ret; }

    inline bool IsString() const {
        return std::holds_alternative<std::string>(data);
    }
    inline bool IsMap() const {
        return std::holds_alternative<std::map<std::string, ADFEntry>>(data);
    }
    inline bool IsArray() const {
        return std::holds_alternative<std::vector<ADFEntry>>(data);
    }
    inline std::string& GetString() {
        if (!IsString()) {
            ADFError("Tried to get a string value from a different type of an ADF entry!");
        }
        return std::get<std::string>(data);
    }
    inline std::map<std::string, ADFEntry>& GetChildren() {
        if (!IsMap()) {
            ADFError("Tried to get a list of children from a different type of an ADF entry!");
        }
        return std::get<std::map<std::string, ADFEntry>>(data);
    }
    inline std::vector<ADFEntry>& GetArray() {
        if (!IsArray()) {
            ADFError("Tried to get an array from a different type of an ADF entry!");
        }
        return std::get<std::vector<ADFEntry>>(data);
    }

    inline const std::string& GetString() const {
        if (!IsString()) {
            ADFError("Tried to get a string value from a different type of an ADF entry!");
        }
        return std::get<std::string>(data);
    }
    inline const std::map<std::string, ADFEntry>& GetChildren() const {
        if (!IsMap()) {
            ADFError("Tried to get a list of children from a different type of an ADF entry!");
        }
        return std::get<std::map<std::string, ADFEntry>>(data);
    }
    inline const std::vector<ADFEntry>& GetArray() const {
        if (!IsArray()) {
            ADFError("Tried to get an array from a different type of an ADF entry!");
        }
        return std::get<std::vector<ADFEntry>>(data);
    }



    inline ADFEntry& operator[](int i) {
        return GetArray()[i];
    }

    inline ADFEntry& operator[](const std::string& name) {
        return GetChildren().at(name);
    }

    inline const ADFEntry& operator[](int i) const {
        return GetArray()[i];
    }

    inline const ADFEntry& operator[](const std::string& name) const {
        return GetChildren().at(name);
    }

    inline bool HasChild(const std::string& name) const {
        return GetChildren().contains(name);
    }

    inline bool HasChildren() const {
        return !GetChildren().empty();
    }

    inline bool HasElements() const {
        return !GetArray().empty();
    }


    //! Used for manual creation of map-type entries that represent a \ref vec2.
    inline static ADFEntry Vector2(const vec2 value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetChildren();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));

        return ret;
    } 
    //! Used for manual creation of map-type entries that represent a \ref vec3.
    inline static ADFEntry Vector3(const vec3 value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetChildren();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));
        retmap.emplace("z", String(std::to_string(value.z)));

        return ret;
    } 
    //! Used for manual creation of map-type entries that represent a \ref vec4.
    inline static ADFEntry Vector4(const vec4 value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetChildren();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));
        retmap.emplace("z", String(std::to_string(value.z)));
        retmap.emplace("w", String(std::to_string(value.w)));

        return ret;
    } 
    //! Used for manual creation of map-type entries that represent a \ref quat.
    inline static ADFEntry Quaternion(const quat value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetChildren();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));
        retmap.emplace("z", String(std::to_string(value.z)));
        retmap.emplace("w", String(std::to_string(value.w)));

        return ret;
    } 

    inline vec2 GetVec2() const {
        const auto& map = GetChildren();

        if (map.contains("x") && map.contains("y")) {
            return vec2(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()));
        }

        ADFError("Tried to get a vec2 from a different type of an ADF entry!");
    }
    inline vec3 GetVec3() const {
        const auto& map = GetChildren();

        if (map.contains("x") && map.contains("y") && map.contains("z")) {
            return vec3(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()), std::stof(map.at("z").GetString()));
        }

        ADFError("Tried to get a vec3 from a different type of an ADF entry!");
    }
    inline vec4 GetVec4() const {
        const auto& map = GetChildren();

        if (map.contains("x") && map.contains("y") && map.contains("z") && map.contains("2")) {
            return vec4(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()), std::stof(map.at("z").GetString()), std::stof(map.at("w").GetString()));
        }

        ADFError("Tried to get a vec4 from a different type of an ADF entry!");
    }
    inline quat GetQuat() const {
        const auto& map = GetChildren();

        if (map.contains("x") && map.contains("y") && map.contains("z") && map.contains("w")) {
            return quat(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()), std::stof(map.at("z").GetString()), std::stof(map.at("w").GetString()));
        }

        ADFError("Tried to get a quaternion from a different type of an ADF entry!");
    }

};