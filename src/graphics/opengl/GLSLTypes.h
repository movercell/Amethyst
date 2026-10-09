#pragma once

#include <array>
#include <map>
#include <string>
#include <string_view>
#include <optional>
#include <engine/filesystem/ADF.h>

enum class GLSLType : int {
    integer = 0,
    uinteger = 1,
    floating = 2,
    dfloating = 3,
    vector2 = 4,
    vector3 = 5,
    vector4 = 6,
    ivector2 = 7,
    ivector3 = 8,
    ivector4 = 9,
    uvector2 = 10,
    uvector3 = 11,
    uvector4 = 12,
    bvector2 = 13,
    bvector3 = 14,
    bvector4 = 15,
    dvector2 = 16,
    dvector3 = 17,
    dvector4 = 18,
    boolean = 19,
    texture = 20
};

inline const constinit std::array<size_t, 21> GLSLType_Align = {
    4,  // integer
    4,  // uinteger 
    4,  // floating 
    8,  // dfloating
    8,  // vector2
    16, // vector3
    16, // vector4
    8,  // ivector2 
    16, // ivector3 
    16, // ivector4 
    8,  // uvector2 
    16, // uvector3 
    16, // uvector4 
    8,  // bvector2 
    16, // bvector3 
    16, // bvector4 
    16, // dvector2 
    32, // dvector3 
    32, // dvector4 
    4,  // boolean
    8   //texture
};

inline const constinit std::array<size_t, 21> GLSLType_Size = {
    4,  // integer
    4,  // uinteger 
    4,  // floating 
    8,  // dfloating
    8,  // vector2
    12, // vector3
    16, // vector4
    8,  // ivector2 
    12, // ivector3 
    16, // ivector4 
    8,  // uvector2 
    12, // uvector3 
    16, // uvector4 
    8,  // bvector2 
    12, // bvector3 
    16, // bvector4 
    16, // dvector2 
    24, // dvector3 
    32, // dvector4 
    4,  // boolean
    8   // texture
};

inline const constinit std::array<std::string_view, 21> GLSLType_Name = {
    "int",    // integer
    "uint",   // uinteger 
    "float",  // floating 
    "double", // dfloating
    "vec2",   // vector2
    "vec3",   // vector3
    "vec4",   // vector4
    "ivec2",  // ivector2
    "ivec3",  // ivector3
    "ivec4",  // ivector4
    "uvec2",  // uvector2
    "uvec3",  // uvector3
    "uvec4",  // uvector4
    "bvec2",  // bvector2
    "bvec3",  // bvector3
    "bvec4",  // bvector4
    "dvec2",  // dvector2
    "dvec3",  // dvector3
    "dvec4",  // dvector4
    "bool",   // boolean
    "texture" // texture
};


class std140BufTemplate {
    struct Member {
        size_t Offset;
        GLSLType Type;
    };
    size_t defaultbuffersize = 4096;
    size_t currentPtr = 0;
    std::byte* buffer = nullptr;
    bool didShrink = false;
    std::map<std::string, Member> members;
public:
    std140BufTemplate() : buffer(new std::byte[defaultbuffersize]) {}
    ~std140BufTemplate() { if (buffer != nullptr) delete[] buffer; }

    void AddMember(std::string_view name, std::string_view type, std::optional<ADFEntry> defaultvalue);
    void Shrink() {
        didShrink = true;

        auto oldbuffer = buffer;
        buffer = new std::byte[currentPtr];
        std::copy(oldbuffer, oldbuffer + currentPtr, buffer);
        delete[] oldbuffer;
    }
    std::unique_ptr<std::byte[]> CreateBuffer() {
        if (!didShrink) {
            Engine::Error("Are you sure you're using this correctly?");
        }
        auto ret = std::make_unique<std::byte[]>(currentPtr);
        std::copy(buffer, &buffer[currentPtr], ret.get());
        return ret;
    }
    size_t GetBufferSize() { return currentPtr; }
    void BufferSetValue(std::unique_ptr<std::byte[]>& Buffer, const std::string& name, ADFEntry& value);

    // Move operators.
    std140BufTemplate(std140BufTemplate&& other) {
        buffer = other.buffer;
        other.buffer = nullptr;
        currentPtr = other.currentPtr;
        didShrink = other.didShrink;
        members = std::move(other.members);
    }
    void operator=(std140BufTemplate&& other) {
        buffer = other.buffer;
        other.buffer = nullptr;
        currentPtr = other.currentPtr;
        didShrink = other.didShrink;
        members = std::move(other.members);
    }
};