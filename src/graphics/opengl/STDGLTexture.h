#pragma once
#include <string>
#include <map>
#include <vector>
#include "glad/glad.h"
#include "engine/Resource.h"
#include "engine/Filesystem.h"
#include "../TextureFile.h"

class STDGLTextureSystem;

class STDGLTexture {
    using MapType = std::map<std::string, Engine::ManagedResource<STDGLTextureSystem, STDGLTexture>*>;
    friend class STDGLTextureSystem;

    GLuint TextureObject;
    GLuint64 BindlessHandle;
    MapType::iterator iteratorintexturemap;
public:
    STDGLTexture(GLuint tex) : TextureObject(tex) {
        if (GLAD_GL_ARB_bindless_texture) {
            BindlessHandle = glGetTextureHandleARB(TextureObject);
            glMakeTextureHandleResidentARB(BindlessHandle);
        }
    };
    ~STDGLTexture() {
        if (TextureObject == 0 ) return;

        if (GLAD_GL_ARB_bindless_texture) {
            glMakeTextureHandleNonResidentARB(BindlessHandle);
        }
        glDeleteTextures(1, &TextureObject);
    };
    STDGLTexture(STDGLTexture&& other) {
        TextureObject = other.TextureObject;
        BindlessHandle = other.BindlessHandle;
        other.TextureObject = 0;
    }
    STDGLTexture& operator=(STDGLTexture&& other) {
        TextureObject = other.TextureObject;
        BindlessHandle = other.BindlessHandle;
        other.TextureObject = 0;

        return *this;
    }
    operator bool() { return TextureObject; }

    GLuint GetTexture() { return TextureObject; }
    GLuint64 GetBindlessHandle() { return BindlessHandle; }
    void Bind(int unit) { glBindTextureUnit(unit, TextureObject); }
};


struct STDGLTextureSystem {
    enum class FilterSettings : uint16_t {
        Point = 0,
        Bilinear = 1,
        Trilinear = 2,
        Anisotropic_2X = 3,
        Anisotropic_4X = 4,
        Anisotropic_8X = 5,
        Anisotropic_16X = 6
    };
    inline static constexpr int FilterSettingsCount = 4;

    STDGLTextureSystem(FilterSettings Filtering = FilterSettings::Trilinear) : filtering(Filtering) {};

    GLuint MakeTextureObject(std::span<char> buffer, const std::string& path);
    Engine::Reference<STDGLTexture> GetTexture(const std::string& name);
    void BindfulOnlySetFiltering(FilterSettings newfilter);

    void _unmanage_resource(Engine::Resource<STDGLTexture>* res) {
        textures.erase(res->Get()->iteratorintexturemap);
        delete res;
    }

private:
    STDGLTexture::MapType textures;
    FilterSettings filtering;

    void SetFilteringOnTexture(GLuint Tex);
};