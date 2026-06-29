#include "STDGLLight.h"
#include <numbers>

void STDGLLight::Bind() {
    if (wasChanged) {
        Info.View = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
        Info.ViewProjection = Info.Projection * Info.View;

        Info.InverseView = glm::inverse(Info.View.toglm());
        Info.InverseViewProjection = glm::inverse(Info.ViewProjection.toglm());

        Info.Frustum = Shapes::Frustum(Info.ViewProjection);
        Info.CameraPos = Position;
        glNamedBufferSubData(Infobuffer, 0, sizeof(STDGLCamera::Camerainfo_t), &Info);

        UpdateData();

        wasChanged = false;
    }
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Infobuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(TextureSpace.PosX, TextureSpace.PosY, TextureSpace.SizeX, TextureSpace.SizeY);
}

void STDGLLight::CreateBuffers() {
    glCreateFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Owner->LightDepthBuffer, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Information buffer
    glCreateBuffers(1, &Infobuffer);
    glNamedBufferStorage(Infobuffer, sizeof(STDGLCamera::Camerainfo_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void STDGLLight::UpdateData() {
    STDGLLightData Data;
    Data.View = Info.View;
    Data.Projection = Info.Projection;
    Data.Type = Type;
    Data.Color = Color;
    Data.FOV = FOV;
    Data.Spot_InnerCutoff = InnerCutoffCosine;
    Data.Spot_OuterCutoff = OuterCutoffCosine;
    Data.Spot_Direction = Front;
    Data.Near = Near;
    Data.Far = Far;
    Data.TextureSpace = TextureSpace;
    Data.LightPos = Position;
    glNamedBufferSubData(Owner->LightDataBuffer, sizeof(STDGLLightData) * ID, sizeof(STDGLLightData), &Data);
}


STDGLLight::STDGLLight(STDGLLightSystem* owner, Engine::Reference<RWorld> rworldref, uint32_t id, STDGLLightType type, vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far) {
    Owner = owner;
    RWorldRef = rworldref;
    ID = id;
    Type = type;
    Resolution = resolution;
    // Don't forget to pad the block to prevent texture bleeding artifacts! 
    TextureSpace = Owner->LightDepthBufferAllocator.AllocPadded(resolution.x, resolution.y, STDGLLIGHT_ALLOC2D_PADDING);
    FOV = outer_cutoff_angle * 2.0f;
    InnerCutoffCosine = cos(inner_cutoff_angle * (std::numbers::pi / 180.0));
    OuterCutoffCosine = cos(outer_cutoff_angle * (std::numbers::pi / 180.0));

    Color = color;
    Near = near;
    Far = far;

    CreateBuffers();

    Info.Projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, Far, Near); // Because reverse-Z.
}

STDGLLight::~STDGLLight() {
    glfwMakeContextCurrent(Owner->Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glDeleteBuffers(1, &Infobuffer);

    // The block has padding that needs to be accounted for when freeing.
    Owner->LightDepthBufferAllocator.FreePadded(TextureSpace, STDGLLIGHT_ALLOC2D_PADDING);
}




Engine::Reference<Light> STDGLLightSystem::MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far) {
    uint32_t ID;
    if (FreedIndices.empty()) {
        ID = NextIndexToMake++;
    } else {
        ID = FreedIndices.front();
        FreedIndices.pop();
    }

    auto* Resource = new Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>(this, this, RWorldRef, ID, Type, resolution, inner_cutoff_angle, outer_cutoff_angle, color, near, far);
    LightResources[ID] = Resource;
    return Engine::Reference(Resource);
}

STDGLLightSystem::STDGLLightSystem() {
    Context = glfwGetCurrentContext();
    LightResources.fill(nullptr);

    glCreateBuffers(1, &LightDataBuffer);

    auto* DefaultData = new STDGLLightData[STDGLLIGHT_MAX_COUNT];
    glNamedBufferData(LightDataBuffer, sizeof(STDGLLightData) * STDGLLIGHT_MAX_COUNT, DefaultData, GL_STATIC_DRAW);
    delete DefaultData;

    // Depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &LightDepthBuffer);
    glTextureStorage2D (LightDepthBuffer, 1, GL_DEPTH_COMPONENT32F, STDGLLIGHT_ALLOC2D_DIMENSTIONS, STDGLLIGHT_ALLOC2D_DIMENSTIONS);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);

    float clearColor = 0.0f;
    glClearTexImage(LightDepthBuffer, 0, GL_RGBA, GL_DEPTH_COMPONENT, &clearColor);
}

STDGLLightSystem::~STDGLLightSystem() {
    glDeleteBuffers(1, &LightDataBuffer);
    glDeleteTextures(1, &LightDepthBuffer);
}