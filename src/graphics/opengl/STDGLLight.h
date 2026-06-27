#pragma once

#include "STDGLCamera.h"
#include "STDGLModel.h"
#include "engine/graphics/Light.h"
#include "engine/Resource.h"
#include "engine/geometry/Alloc2D.h"

inline constexpr int STDGLLIGHT_MAX_COUNT = 4096;
inline constexpr int STDGLLIGHT_ALLOC2D_DIMENSTIONS = 8192;
inline constexpr int STDGLLIGHT_ALLOC2D_PADDING = 4;

struct STDGLLightSystem;

enum class STDGLLightType : GLuint {
    None = 0,
    Spot = 1
};

struct STDGLLightData {
    mat4 ViewProjection;
    Geometry::Alloc2D::Block TextureSpace;
    STDGLLightType Type = STDGLLightType::None;
    float FOV;
    float Near;
    float Far;
    vec3 Color;
    vec3 LightPos;
};

struct RWorld;

struct STDGLLight : public Light {
    GLuint Framebuffer;
    GLuint Infobuffer;
    STDGLLightSystem* Owner;
    STDGLCamera::Camerainfo_t Info;
    STDGLLightType Type;
    uint32_t ID;
    Engine::Reference<RWorld> RWorldRef;
    Geometry::Alloc2D::Block TextureSpace;


    STDGLLight(STDGLLightSystem* owner, Engine::Reference<RWorld> rworldref, uint32_t id, STDGLLightType type, Geometry::Alloc2D::Block texturespace, float fov, vec3 color, float near, float far) {
        Owner = owner;
        RWorldRef = rworldref;
        ID = id;
        Type = type;
        TextureSpace = texturespace;
        Resolution = vec2(TextureSpace.SizeX, TextureSpace.SizeY);
        FOV = fov;
        Color = color;
        Near = near;
        Far = far;

        CreateBuffers();

        wasChanged = true;
    }
    ~STDGLLight();

    void Bind();
protected:
    void CreateBuffers();
    void UpdateData();
};


struct STDGLLightSystem {
    std::queue<uint32_t> FreedIndices;
    uint32_t NextIndexToMake = 0;

    std::array<Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>*, STDGLLIGHT_MAX_COUNT> LightResources;

    GLFWwindow* Context = 0;
    GLuint LightDataBuffer = 0;
    GLuint LightDepthBuffer = 0;

    Geometry::Alloc2D LightDepthBufferAllocator = { STDGLLIGHT_ALLOC2D_DIMENSTIONS, STDGLLIGHT_ALLOC2D_DIMENSTIONS };

    Engine::Reference<Light> MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float fov, vec3 color, float near, float far);

    STDGLLightSystem() {
        Context = glfwGetCurrentContext();
        LightResources.fill(nullptr);

        glCreateBuffers(1, &LightDataBuffer);

        auto* DefaultData = new STDGLLightData[STDGLLIGHT_MAX_COUNT];
        glNamedBufferData(LightDataBuffer, sizeof(STDGLLightData) * STDGLLIGHT_MAX_COUNT, DefaultData, GL_STATIC_DRAW);
        delete DefaultData;

        // Depth buffer
        glCreateTextures(GL_TEXTURE_2D, 1, &LightDepthBuffer);
        glTextureStorage2D (LightDepthBuffer, 1, GL_DEPTH_COMPONENT16, STDGLLIGHT_ALLOC2D_DIMENSTIONS, STDGLLIGHT_ALLOC2D_DIMENSTIONS);
        glTextureParameteri(LightDepthBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(LightDepthBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(LightDepthBuffer, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTextureParameteri(LightDepthBuffer, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glClearTexImage(LightDepthBuffer, 0, GL_RGBA, GL_FLOAT, clearColor);
    }

    ~STDGLLightSystem() {
        glDeleteBuffers(1, &LightDataBuffer);
        glDeleteTextures(1, &LightDepthBuffer);
    }

    void _unmanage_resource(Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>* res) {
        glfwMakeContextCurrent(Context);

        uint32_t ID = res->resource.ID;
        LightResources[ID] = nullptr;

        STDGLLightData EmptyData;
        glNamedBufferSubData(LightDataBuffer, sizeof(STDGLLightData) * ID, sizeof(STDGLLightData), &EmptyData);

        
        FreedIndices.push(ID);
        if (FreedIndices.size() >= NextIndexToMake) {
            std::queue<uint32_t> empty;
            FreedIndices.swap(empty);
            NextIndexToMake = 0;
        }

        delete res;
    }

    void Bind() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, LightDataBuffer);
        glBindTextureUnit(15, LightDepthBuffer);
    }
};