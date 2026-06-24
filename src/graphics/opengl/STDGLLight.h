#pragma once

#include "STDGLCamera.h"
#include "STDGLModel.h"
#include "engine/graphics/Light.h"
#include "engine/Resource.h"

inline constexpr int STDGLLIGHT_MAX_COUNT = STDGLMODEL_INSTANCE_MAX_COUNT;

struct STDGLLightSystem;

enum class STDGLLightType : GLuint {
    None,
    Spot
};

struct STDGLLightData {
    mat4 ViewProjection;
    GLuint64 Texture = 0;
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
    GLuint Depthbuffer;
    GLuint Infobuffer;
    GLuint Depthhandle;
    STDGLLightSystem* Owner;
    STDGLCamera::Camerainfo_t Info;
    STDGLLightType Type;
    uint32_t ID;
    Engine::Reference<RWorld> RWorldRef;


    STDGLLight(STDGLLightSystem* owner, Engine::Reference<RWorld> rworldref, uint32_t id, STDGLLightType type, vec2 resolution, float fov, vec3 color, float near, float far) {
        Owner = owner;
        RWorldRef = rworldref;
        ID = id;
        Resolution = resolution;
        FOV = fov;
        Color = color;
        Near = near;
        Far = far;

        CreateBuffers();

        wasChanged = true;
    }
    ~STDGLLight();

    void Bind();

    GLuint GetHandle() {
        return Depthhandle;
    }
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

    Engine::Reference<Light> MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float fov, vec3 color, float near, float far);

    STDGLLightSystem() {
        Context = glfwGetCurrentContext();
        LightResources.fill(nullptr);

        glCreateBuffers(1, &LightDataBuffer);

        auto* DefaultData = new STDGLLightData[STDGLLIGHT_MAX_COUNT];
        glNamedBufferData(LightDataBuffer, sizeof(STDGLLightData) * STDGLLIGHT_MAX_COUNT, DefaultData, GL_STATIC_DRAW);
        delete DefaultData;
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
};