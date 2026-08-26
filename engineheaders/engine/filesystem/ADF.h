#pragma once
#include "engine/filesystem/Filesystem.h"
#include "engine/master.h"
#include "engine/Resource.h"
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <inplace_vector>
#include <sstream>
#include <meta>
#include <type_traits>

//! Tag for specified ADF serialization.
namespace Engine {
    namespace Internal {
        constexpr struct {} ADFSerializeAnnotation;
    }
}

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

    class Tokenizer {
        std::istream* stream;
        const std::string& filepath;
        static constexpr auto eof = std::char_traits<char>::eof();

        TokenType CurrentType;
        std::inplace_vector<char, 256> CurrentContent;
    public:
        Tokenizer(std::istream* Stream, const std::string& FilePath) : filepath(FilePath), stream(std::move(Stream)) {}
        void ReadToken();

        TokenType GetCurrentTokenType() { return CurrentType; }
        std::string GetCurrentTokenContent() { return {CurrentContent.data(), CurrentContent.size()}; }
    };

    
    std::variant<std::map<std::string, ADFEntry>, std::string, std::vector<ADFEntry>> data;
    // This is used for showing which file an error came from
    Engine::Reference<std::string> Filename;

    [[noreturn]] void ADFError(const std::string& error) const;

    ENGINEEXPORT void ToStream(std::streambuf* buffer, int IndentationLevel) const;
    void ToStreamObjectFormatHelper(std::streambuf* buffer, int IndentationLevel) const;
    void ToStreamStringFormatHelper(std::streambuf* buffer, const std::string& str) const;

    ENGINEEXPORT void ToStreamCompact(std::streambuf* buffer) const;
    void ToStreamCompactObjectFormatHelper(std::streambuf* buffer) const;

    ADFEntry(ADFType Type, Tokenizer& Tokenizer, Engine::Reference<std::string> filename);
    ADFEntry(std::string content, Engine::Reference<std::string> filename) { data = std::move(content); filename = filename; }
public:
    //! Creates an ADF tree from a .adf file.
    static ENGINEEXPORT ADFEntry FromFile(const std::string& FilePath);
    //! Creates an ADF tree from a stream.
    static ENGINEEXPORT ADFEntry FromStream(std::istream& Stream);
    void ToFile(const std::string& FilePath, bool isCompact = false) const {
        if (!IsMap()) {
            Engine::Error("Attempted to turn a non-Map-type ADF entry into a string, only a Map-type entry can be the root node of a tree!");
        }
        auto out = Filesystem::GetFileOutputStream(FilePath, std::ios::binary);

        auto sentry = std::ofstream::sentry(out);
        if (!sentry) {
            Engine::Error("Failed to create an output stream for an .ADF export!");
        }

        auto buffer = out.rdbuf();
        if (isCompact) {
            ToStreamCompact(buffer);
        } else {
            ToStream(buffer, 0);
        }
    }
    void ToStream(std::ostream Stream, bool isCompact = false) const {
        auto sentry = std::ostream::sentry(Stream);
        if (!sentry) {
            Engine::Error("Failed to lock the output stream for an .ADF export!");
        }

        auto buffer = Stream.rdbuf();
        if (isCompact) {
            ToStreamCompact(buffer);
        } else {
            ToStream(buffer, 0);
        }
    }

    ADFEntry() {};
    //! Used for manual creation of string-type entries.
    static ADFEntry String(std::string Content = std::string()) { ADFEntry ret; ret.data = Content; return ret; }
    //! Used for manual creation of map-type entries.
    static ADFEntry Map(std::map<std::string, ADFEntry> Content = std::map<std::string, ADFEntry>()) { ADFEntry ret; ret.data = Content; return ret; }
    //! Used for manual creation of array-type entries.
    static ADFEntry Array(std::vector<ADFEntry> Content = std::vector<ADFEntry>()) { ADFEntry ret; ret.data = Content; return ret; }

    bool IsString() const {
        return std::holds_alternative<std::string>(data);
    }
    bool IsMap() const {
        return std::holds_alternative<std::map<std::string, ADFEntry>>(data);
    }
    bool IsArray() const {
        return std::holds_alternative<std::vector<ADFEntry>>(data);
    }
    std::string& GetString() {
        if (!IsString()) {
            ADFError("Tried to get a string from a different type of an ADF entry!");
        }
        return std::get<std::string>(data);
    }
    std::map<std::string, ADFEntry>& GetMap() {
        if (!IsMap()) {
            ADFError("Tried to get a map from a different type of an ADF entry!");
        }
        return std::get<std::map<std::string, ADFEntry>>(data);
    }
    std::vector<ADFEntry>& GetArray() {
        if (!IsArray()) {
            ADFError("Tried to get an array from a different type of an ADF entry!");
        }
        return std::get<std::vector<ADFEntry>>(data);
    }

    const std::string& GetString() const {
        if (!IsString()) {
            ADFError("Tried to get a string value from a different type of an ADF entry!");
        }
        return std::get<std::string>(data);
    }
    const std::map<std::string, ADFEntry>& GetMap() const {
        if (!IsMap()) {
            ADFError("Tried to get a list of children from a different type of an ADF entry!");
        }
        return std::get<std::map<std::string, ADFEntry>>(data);
    }
    const std::vector<ADFEntry>& GetArray() const {
        if (!IsArray()) {
            ADFError("Tried to get an array from a different type of an ADF entry!");
        }
        return std::get<std::vector<ADFEntry>>(data);
    }



    ADFEntry& operator[](int i) {
        return GetArray()[i];
    }
    ADFEntry& operator[](const std::string& name) {
        return GetMap().at(name);
    }
    const ADFEntry& operator[](int i) const {
        return GetArray()[i];
    }
    const ADFEntry& operator[](const std::string& name) const {
        return GetMap().at(name);
    }

    bool HasChild(const std::string& name) const {
        return GetMap().contains(name);
    }
    bool HasChildren() const {
        return !GetMap().empty();
    }
    bool HasElements() const {
        return !GetArray().empty();
    }

    bool operator==(const ADFEntry& other) const {
        return data == other.data;
    }
    bool operator==(const ADFEntry&& other) const {
        return data == other.data;
    }


    //! Used for manual creation of map-type entries that represent a \ref vec2.
    static ADFEntry Vector2(const vec2 value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetMap();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));

        return ret;
    } 
    //! Used for manual creation of map-type entries that represent a \ref vec3.
    static ADFEntry Vector3(const vec3 value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetMap();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));
        retmap.emplace("z", String(std::to_string(value.z)));

        return ret;
    } 
    //! Used for manual creation of map-type entries that represent a \ref vec4.
    static ADFEntry Vector4(const vec4 value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetMap();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));
        retmap.emplace("z", String(std::to_string(value.z)));
        retmap.emplace("w", String(std::to_string(value.w)));

        return ret;
    } 
    //! Used for manual creation of map-type entries that represent a \ref quat.
    static ADFEntry Quaternion(const quat value) {
        ADFEntry ret = Map();
        auto& retmap = ret.GetMap();

        retmap.emplace("x", String(std::to_string(value.x)));
        retmap.emplace("y", String(std::to_string(value.y)));
        retmap.emplace("z", String(std::to_string(value.z)));
        retmap.emplace("w", String(std::to_string(value.w)));

        return ret;
    } 

    //! Interprets a map-type entry as a 2-component vector.
    vec2 GetVec2() const {
        const auto& map = GetMap();

        if (map.contains("x") && map.contains("y")) {
            return vec2(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()));
        }

        ADFError("Tried to get a vec2 from a different type of an ADF entry!");
    }
    //! Interprets a map-type entry as a 3-component vector.
    vec3 GetVec3() const {
        const auto& map = GetMap();

        if (map.contains("x") && map.contains("y") && map.contains("z")) {
            return vec3(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()), std::stof(map.at("z").GetString()));
        }

        ADFError("Tried to get a vec3 from a different type of an ADF entry!");
    }
    //! Interprets a map-type entry as a 4-component vector.
    vec4 GetVec4() const {
        const auto& map = GetMap();

        if (map.contains("x") && map.contains("y") && map.contains("z") && map.contains("2")) {
            return vec4(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()), std::stof(map.at("z").GetString()), std::stof(map.at("w").GetString()));
        }

        ADFError("Tried to get a vec4 from a different type of an ADF entry!");
    }
    //! Interprets a map-type entry as a quaternion(Interpreted as euler angles and converted to a quaternion if a W component is not present.).
    quat GetQuat() const {
        const auto& map = GetMap();

        if (map.contains("x") && map.contains("y") && map.contains("z")) {
            if (map.contains("w")) {
                return quat(std::stof(map.at("x").GetString()), std::stof(map.at("y").GetString()), std::stof(map.at("z").GetString()), std::stof(map.at("w").GetString()));
            }
            return quat(GetVec3()); // Read as euler angles if there's no w.
        }

        ADFError("Tried to get a quaternion from a different type of an ADF entry!");
    }



    // Reflection-based API for easy storing/loading of structures.

    //! Recursively converts an object to an ADFEntry using reflection.(Only converts members that have the ADFSerialize annotation, and if no members have it implicitly stores everything.)
    template<typename T>
    static ADFEntry Serialize(const T& Object) {
        if constexpr (std::meta::is_pointer_type(^^T)) {
            static_assert(false, "ADFEntry::Serialize: Attempted to serialize a pointer, nope.");
        }

        constexpr auto TClean = std::meta::dealias(^^T);
        constexpr auto IsTemplate = std::meta::has_template_arguments(TClean);
        constexpr bool IsVector = [IsTemplate, TClean]() -> bool {
            if constexpr (IsTemplate) {
                constexpr bool res = std::meta::template_of(TClean) == std::meta::dealias(^^std::vector);
                return res;
            } else { return false; }
        }();
        constexpr bool IsArray = [IsTemplate, TClean]() -> bool {
            if constexpr (IsTemplate) {
                constexpr bool res = std::meta::template_of(TClean) == std::meta::dealias(^^std::array);
                return res;
            } else { return false; }
        }();
        constexpr bool IsMap = [IsTemplate, TClean]() -> bool {
            if constexpr (IsTemplate) {
                constexpr bool res = (std::meta::template_of(TClean) == std::meta::dealias(^^std::map)) || 
                                     (std::meta::template_of(TClean) == std::meta::dealias(^^std::unordered_map));
                return res;
            } else { return false; }
        }();

        if constexpr (TClean == ^^ADFEntry) {
            // The object is already an ADFEntry.
            return Object;

        } else if constexpr (TClean == ^^bool) {
            // The object is a boolean.
            return String(Object ? "1" : "0");

        } else if constexpr (TClean == std::meta::dealias(^^std::string)) {
            // The object is a string.
            return String(Object);

        } else if constexpr (requires(std::remove_const_t<T> testobj) {std::from_chars(nullptr, nullptr, testobj);}) { // It's from_chars because to_string can invoke a conversion, especially for object that have an `operator bool`.
            // The object can be converted to a string.
            return String(std::to_string(Object));

        } else if constexpr (TClean == ^^vec2) {
            // The object is a 2-component vector.
            return Vector2(Object);

        } else if constexpr (TClean == ^^vec3) {
            // The object is a 3-component vector.
            return Vector3(Object);

        } else if constexpr (TClean == ^^vec4) {
            // The object is a 4-component vector.
            return Vector4(Object);

        } else if constexpr (TClean == ^^quat) {
            // The object is a quaternion.
            return Quaternion(Object);

        } else if constexpr (IsArray || IsVector) {
            // The object is a std::vector or a std::array.
            ADFEntry ret = ADFEntry::Array();
            auto& retarray = ret.GetArray();

            retarray.reserve(Object.size());
            for (auto& Element : Object) {
                retarray.emplace_back(Serialize(Element));
            }

            return ret;
        
        } else if constexpr (IsMap) {
            // The object is a std::map<std::string, T>.
            static_assert(std::meta::template_arguments_of(TClean)[0] == std::meta::dealias(^^std::string), "ADFEntry::Serialize: A serialized map must have it's key be std::string!");
            ADFEntry ret = ADFEntry::Map();
            auto& retmap = ret.GetMap();

            for (const auto& Element : Object) {
                retmap.emplace(Element.first, ADFEntry::Serialize(Element.second));
            }

            return ret;

        } else if constexpr (std::is_class_v<T> && !std::is_union_v<T>) {
            // The object is a structure.
            ADFEntry ret = ADFEntry::Map();
            auto& retmap = ret.GetMap();
            
            static constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(TClean, std::meta::access_context::unchecked()));

            // Check if none of the members have the ADFSerialize annotation, then implicitly store everything.
            constexpr bool ImplicitStoreAll = std::none_of(Members.begin(), Members.end(), [](std::meta::info Member) -> bool {
                return std::meta::annotations_of_with_type(Member, ^^decltype(Engine::Internal::ADFSerializeAnnotation)).size() > 0;
            });

            template for (constexpr auto Member : Members) {
                if constexpr (ImplicitStoreAll || (std::meta::annotations_of_with_type(Member, ^^decltype(Engine::Internal::ADFSerializeAnnotation)).size() > 0)) {
                    retmap.emplace(std::meta::identifier_of(Member), Serialize(Object.[:Member:]));
                }
            }

            // Now, handle parent classes.
            static constexpr auto Bases = std::define_static_array(std::meta::bases_of(TClean, std::meta::access_context::unchecked()));
            template for (constexpr auto Base : Bases) {
                using BaseType = [:std::meta::type_of(Base):];
                ADFEntry SerializedBase = Serialize<BaseType>(Object);
                retmap.merge(SerializedBase.GetMap());
            }

            return ret;

        } else {
            static_assert(false, "ADFEntry::Serialize: Cannot turn object into an ADFEntry!");
        }
    }

    //! Recursively converts an ADFEntry to an object using reflection.(Only converts members that have the ADFSerialize annotation, and if no members have it implicitly loads everything.)
    template<typename T>
    void Deserialize(T& Object) const {
        if constexpr (std::meta::is_pointer_type(^^T)) {
            static_assert(false, "ADFEntry::Deserialize: Attempted to deserialize a pointer, nope.");
        }

        constexpr auto TClean = std::meta::dealias(^^T);
        constexpr auto IsTemplate = std::meta::has_template_arguments(TClean);
        constexpr bool IsVector = [IsTemplate, TClean]() -> bool {
            if constexpr (IsTemplate) {
                constexpr bool res = std::meta::template_of(TClean) == std::meta::dealias(^^std::vector);
                return res;
            } else { return false; }
        }();
        constexpr bool IsArray = [IsTemplate, TClean]() -> bool {
            if constexpr (IsTemplate) {
                constexpr bool res = std::meta::template_of(TClean) == std::meta::dealias(^^std::array);
                return res;
            } else { return false; }
        }();
        constexpr bool IsMap = [IsTemplate, TClean]() -> bool {
            if constexpr (IsTemplate) {
                constexpr bool res = (std::meta::template_of(TClean) == std::meta::dealias(^^std::map)) || 
                                     (std::meta::template_of(TClean) == std::meta::dealias(^^std::unordered_map));
                return res;
            } else { return false; }
        }();

        if constexpr (TClean == ^^ADFEntry) {
            // The object is already an ADFEntry.
            Object = *this;

        } else if constexpr (TClean == ^^bool) {
            // The object is a boolean.
            Object = GetString() == "1";

        } else if constexpr (TClean == std::meta::dealias(^^std::string)) {
            // The object is a string.
            Object = GetString();

        } else if constexpr (requires {std::from_chars(nullptr, nullptr, Object);}) {
            // The object can be converted from a string.
            std::string string = GetString();
            std::from_chars(string.data(), string.data() + string.size(), Object);

        } else if constexpr (TClean == ^^vec2) {
            // The object is a 2-component vector.
            Object = GetVec2();

        } else if constexpr (TClean == ^^vec3) {
            // The object is a 3-component vector.
            Object = GetVec3();

        } else if constexpr (TClean == ^^vec4) {
            // The object is a 4-component vector.
            Object = GetVec4();

        } else if constexpr (TClean == ^^quat) {
            // The object is a quaternion.
            Object = GetQuat();

        } else if constexpr (IsArray || IsVector) {
            // The object is a std::vector or a std::array.
            auto& array = GetArray();
            int sizetofill;

            if constexpr (IsVector) {
                sizetofill = array.size();
                Object.resize(sizetofill);
                Object.shrink_to_fit();
            }
            if constexpr (IsArray) {
                sizetofill = std::min(Object.size(), array.size());
            }
            for (int i = 0; i < sizetofill; i++) {
                array[i].Deserialize(Object[i]); 
            }

        } else if constexpr (IsMap) {
            // The object is a std::map<std::string, T>.
            static_assert(std::meta::template_arguments_of(TClean)[0] == std::meta::dealias(^^std::string), "ADFEntry::Deserialize: A serialized map must have it's key be std::string!");
            
            auto& map = GetMap();
            constexpr auto typeinfo = std::meta::template_arguments_of(TClean)[1];
            using type = [:typeinfo:];

            Object.clear();
            for (auto& Element : map) {
                type reselement;
                Element.second.Deserialize(reselement);

                Object.emplace(Element.first, reselement);
            }

        } else if constexpr (std::is_class_v<T> && !std::is_union_v<T>) {
            // The object is a structure.
            auto& map = GetMap();
            
            static constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(TClean, std::meta::access_context::unchecked()));

            // Check if none of the members have the ADFSerialize annotation, then implicitly store everything.
            constexpr bool ImplicitLoadAll = std::none_of(Members.begin(), Members.end(), [](std::meta::info Member) -> bool {
                return std::meta::annotations_of_with_type(Member, ^^decltype(Engine::Internal::ADFSerializeAnnotation)).size() > 0;
            });

            template for (constexpr auto Member : Members) {
                if constexpr (ImplicitLoadAll || (std::meta::annotations_of_with_type(Member, ^^decltype(Engine::Internal::ADFSerializeAnnotation)).size() > 0)) {
                    try {
                        map.at(std::string(std::meta::identifier_of(Member))).Deserialize(Object.[:Member:]);
                    } catch (const std::out_of_range& e) {}
                }
            }

            // Now, handle parent classes.
            static constexpr auto Bases = std::define_static_array(std::meta::bases_of(TClean, std::meta::access_context::unchecked()));
            template for (constexpr auto Base : Bases) {
                using BaseType = [:std::meta::type_of(Base):];
                Deserialize<BaseType>(Object);
            }

        } else {
            static_assert(false, "ADFEntry::Deserialize: Cannot turn an ADFEntry into the object!");
        }
    }
};

#ifdef __INTELLISENSE__ 
#define ADFSerialize
#else
#define ADFSerialize [[= Engine::Internal::ADFSerializeAnnotation ]]
#endif