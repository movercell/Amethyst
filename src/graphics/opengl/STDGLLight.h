#pragma once

#include "STDGLCamera.h"
#include "STDGLModel.h"
#include "engine/graphics/Light.h"
#include "engine/Resource.h"
#include "engine/geometry/Alloc2D.h"

inline constexpr int STDGLLIGHT_MAX_COUNT = 1024;
inline constexpr int STDGLLIGHT_ALLOC2D_DIMENSTIONS = 8192;
inline constexpr int STDGLLIGHT_ALLOC2D_PADDING = 4;

struct STDGLLightSystem;

enum class STDGLLightType : GLuint {
    None = 0,
    Spot = 1,
    Point = 2
};

struct STDGLLightData {
    mat4 View;
    mat4 Projection;
    Geometry::Alloc2D::Block TextureSpace;
    STDGLLightType Type = STDGLLightType::None;
    float FOV;
    float Near;
    float Far;
    float Falloff_Constant;
    float Falloff_Linear;
    float Falloff_Quadratic;
    float Intensity;
    float Spot_InnerCutoff; // Cosines of the angles.
    float Spot_OuterCutoff; // Cosines of the angles.
    vec3 Color;
    vec3 LightPos;
    vec3 Spot_Direction;
};

struct RWorld;

struct STDGLLight : public Light {
    GLuint Framebuffer;
    GLuint Infobuffer;
    float InnerCutoffCosine;
    float OuterCutoffCosine;
    vec3 Falloff = vec3(0.0f, 0.0f, 1.0f); // Purely quadratic.
    STDGLLightSystem* Owner;
    STDGLCamera::Camerainfo_t Info;
    STDGLLightType Type;
    uint32_t ID;
    float Intensity = 1000.0f;
    Engine::Reference<RWorld> RWorldRef;
    Geometry::Alloc2D::Block TextureSpace;


    STDGLLight(STDGLLightSystem* owner, Engine::Reference<RWorld> rworldref, uint32_t id, STDGLLightType type, vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far);
    ~STDGLLight();

    void Bind();

    void SetPosition(vec3 Position);
    void SetRotation(quat Rotation);
    void SetColor(vec3 color);
    void SetInnerAngle(float Inner);
    void SetOuterAngle(float Outer);
    void SetFalloff(float Constant, float Linear, float Quadraric);
    void SetIntensity(float Intensity);
protected:
    void CreateBuffers();
    void UpdateData();
    
    bool wasChanged = true;
    bool wasChangedProjection = true;
};


struct STDGLLightSystem {
    std::queue<uint32_t> FreedIndices;
    uint32_t NextIndexToMake = 0;

    std::array<Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>*, STDGLLIGHT_MAX_COUNT> LightResources;

    GLFWwindow* Context = 0;
    GLuint LightDataBuffer = 0;
    GLuint LightDepthBuffer = 0;

    Geometry::Alloc2D LightDepthBufferAllocator = { STDGLLIGHT_ALLOC2D_DIMENSTIONS, STDGLLIGHT_ALLOC2D_DIMENSTIONS };

    Engine::Reference<Light> MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far);

    STDGLLightSystem();

    ~STDGLLightSystem();

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