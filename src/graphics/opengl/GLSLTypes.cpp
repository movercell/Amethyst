#include "GLSLTypes.h"
#include <algorithm>
#include <format>
#include "engine/master.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "../TextureFile.h"

inline constexpr int STD140BUF_MAX_TEXTURES = 12;

static ADFEntry GetDefaultValueForType(GLSLType Type) {
    switch (Type) {
    case GLSLType::integer:
        return ADFEntry::String("0");
    case GLSLType::uinteger:
        return ADFEntry::String("0");
    case GLSLType::floating:
        return ADFEntry::String("0.0f");
    case GLSLType::dfloating:
        return ADFEntry::String("0.0f");

    case GLSLType::vector2:
        return ADFEntry::Serialize(glm::vec2());
    case GLSLType::vector3:
        return ADFEntry::Serialize(glm::vec3());
    case GLSLType::vector4:
        return ADFEntry::Serialize(glm::vec4());

    case GLSLType::ivector2:
        return ADFEntry::Serialize(glm::ivec2());
    case GLSLType::ivector3:
        return ADFEntry::Serialize(glm::ivec3());
    case GLSLType::ivector4:
        return ADFEntry::Serialize(glm::ivec4());

    case GLSLType::uvector2:
        return ADFEntry::Serialize(glm::uvec2());
    case GLSLType::uvector3:
        return ADFEntry::Serialize(glm::uvec3());
    case GLSLType::uvector4:
        return ADFEntry::Serialize(glm::uvec4());

    case GLSLType::bvector2:
        return ADFEntry::Serialize(glm::bvec2());
    case GLSLType::bvector3:
        return ADFEntry::Serialize(glm::bvec3());
    case GLSLType::bvector4:
        return ADFEntry::Serialize(glm::bvec4());

    case GLSLType::dvector2:
        return ADFEntry::Serialize(glm::dvec2());
    case GLSLType::dvector3:
        return ADFEntry::Serialize(glm::dvec3());
    case GLSLType::dvector4:
        return ADFEntry::Serialize(glm::dvec4());
        
    case GLSLType::boolean:
        return ADFEntry::String("0");
    case GLSLType::texture:
        return ADFEntry::String("14");
    }

    Engine::Error("my bad");
}

static void SetValueInBufferForType(GLSLType Type, void* Place, ADFEntry& Value) {
    switch (Type) {
    case GLSLType::integer:
        Value.Deserialize(*reinterpret_cast<GLint*>(Place));
        break;
    case GLSLType::uinteger:
        Value.Deserialize(*reinterpret_cast<GLuint*>(Place));
        break;
    case GLSLType::floating:
        Value.Deserialize(*reinterpret_cast<GLfloat*>(Place));
        break;
    case GLSLType::dfloating:
        Value.Deserialize(*reinterpret_cast<GLdouble*>(Place));
        break;

    case GLSLType::vector2:
        Value.Deserialize(*reinterpret_cast<glm::vec2*>(Place));
        break;
    case GLSLType::vector3:
        Value.Deserialize(*reinterpret_cast<glm::vec3*>(Place));
        break;
    case GLSLType::vector4:
        Value.Deserialize(*reinterpret_cast<glm::vec4*>(Place));
        break;

    case GLSLType::ivector2:
        Value.Deserialize(*reinterpret_cast<glm::ivec2*>(Place));
        break;
    case GLSLType::ivector3:
        Value.Deserialize(*reinterpret_cast<glm::ivec3*>(Place));
        break;
    case GLSLType::ivector4:
        Value.Deserialize(*reinterpret_cast<glm::ivec4*>(Place));
        break;

    case GLSLType::uvector2:
        Value.Deserialize(*reinterpret_cast<glm::uvec2*>(Place));
        break;
    case GLSLType::uvector3:
        Value.Deserialize(*reinterpret_cast<glm::uvec3*>(Place));
        break;
    case GLSLType::uvector4:
        Value.Deserialize(*reinterpret_cast<glm::uvec4*>(Place));
        break;

    case GLSLType::bvector2:
        Value.Deserialize(*reinterpret_cast<glm::bvec2*>(Place));
        break;
    case GLSLType::bvector3:
        Value.Deserialize(*reinterpret_cast<glm::bvec3*>(Place));
        break;
    case GLSLType::bvector4:
        Value.Deserialize(*reinterpret_cast<glm::bvec4*>(Place));
        break;

    case GLSLType::dvector2:
        Value.Deserialize(*reinterpret_cast<glm::dvec2*>(Place));
        break;
    case GLSLType::dvector3:
        Value.Deserialize(*reinterpret_cast<glm::dvec3*>(Place));
        break;
    case GLSLType::dvector4:
        Value.Deserialize(*reinterpret_cast<glm::dvec4*>(Place));
        break;
        
    case GLSLType::boolean:
        Value.Deserialize(*reinterpret_cast<GLboolean*>(Place));
        break;
    case GLSLType::texture:
        Value.Deserialize(*reinterpret_cast<GLuint64*>(Place));
        break;
    }
}

void std140BufTemplate::AddMember(std::string_view name, std::string_view type, std::optional<ADFEntry> defaultvalue) {
    if (didShrink) {
        Engine::Warning("Attempted to add a member to a shrunk std140BufTemplate!");
        return;
    }

    GLSLType Type;
    int TextureIndex = 0;
    {
        auto it = std::find(GLSLType_Name.begin(), GLSLType_Name.end(), type);
        if (it == GLSLType_Name.end()) {
            Engine::Warning(std::format("Not a GLSL type: {}", type));
            return;
        }
        Type = (GLSLType)std::distance(GLSLType_Name.begin(), it);
    }
    if (Type == GLSLType::texture) {
        TextureIndex = texCount++;
        if (texCount > STD140BUF_MAX_TEXTURES) {
            Engine::Error("std140BufTemplate: Buffer exceeded STD140BUF_MAX_TEXTURES textures!");
        }
    }
    size_t Alignment = GLSLType_Align[(int)Type];
    size_t Size = GLSLType_Size[(int)Type];

    size_t Ptr = (currentPtr + (Alignment - 1)) & ~(Alignment - 1);
    currentPtr = Ptr + Size;

    if (defaultvalue && (Type == GLSLType::texture)) {
        Engine::Warning("A shader default value may not be applied to a texture!");
    }
    if (defaultvalue && (Type != GLSLType::texture)) {
        SetValueInBufferForType(Type, buffer + Ptr, defaultvalue.value());
    } else {
        auto Default = GetDefaultValueForType(Type);
        SetValueInBufferForType(Type, buffer + Ptr, Default);
    }

    members.emplace(name, Member(Ptr, Type, TextureIndex));
}
void std140BufTemplate::BufferSetValue(std140Buf& Buffer, const std::string& name, ADFEntry& value, std::optional<STDGLTextureSystem*> TextureSystem) {
    auto it = members.find(name);
    if (it != members.end()) {
        if (it->second.Type == GLSLType::texture) {
            if (!TextureSystem.has_value()) {
                Engine::Error("std140BufTemplate::BufferSetValue: Attempted to add a texture without providing a pointer to a texture system!");
            }

            auto texsystem = TextureSystem.value();
            auto tex = texsystem->GetTexture(value.GetString());

            Buffer.textureobjrefs[it->second.TextureIndex] = tex;
            Buffer.textures[it->second.TextureIndex] = tex.get()->GetTexture();

            auto texvalue = ADFEntry::String(std::to_string(GLAD_GL_ARB_bindless_texture ? tex.get()->GetBindlessHandle() : it->second.TextureIndex));
            SetValueInBufferForType(GLSLType::texture, Buffer.buffer.get() + it->second.Offset, texvalue);
            return;
        }

        SetValueInBufferForType(it->second.Type, Buffer.buffer.get() + it->second.Offset, value);
    }
}