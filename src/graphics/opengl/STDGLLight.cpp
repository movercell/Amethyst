#include "STDGLLight.h"

void STDGLLight::Bind() {
    if (wasChanged) {
        Info.View = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
        mat4 projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, Near, Far);
        Info.ViewProjection = projection * Info.View;

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
}

void STDGLLight::CreateBuffers() {
    glCreateFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    // Depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &Depthbuffer);
    glTextureStorage2D(Depthbuffer, 1, GL_DEPTH_COMPONENT16, Resolution.x, Resolution.y);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);


    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(Depthbuffer, GL_TEXTURE_BORDER_COLOR, borderColor);

    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Depthbuffer, 0);

    Depthhandle = glGetTextureHandleARB(Depthbuffer);
    glMakeTextureHandleResidentARB(Depthhandle);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Information buffer
    glCreateBuffers(1, &Infobuffer);
    glNamedBufferStorage(Infobuffer, sizeof(STDGLCamera::Camerainfo_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void STDGLLight::UpdateData() {
    STDGLLightData Data;
    Data.ViewProjection = Info.ViewProjection;
    Data.Type = Type;
    Data.Color = Color;
    Data.Far = Far;
    Data.FOV = FOV;
    Data.Near = Near;
    Data.Texture = Depthhandle;
    Data.LightPos = Position;
    glNamedBufferSubData(Owner->LightDataBuffer, sizeof(STDGLLightData) * ID, sizeof(STDGLLightData), &Data);
}

STDGLLight::~STDGLLight() {
    glfwMakeContextCurrent(Owner->Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glMakeTextureHandleNonResidentARB(Depthhandle);
    glDeleteTextures(1, &Depthbuffer);
    glDeleteBuffers(1, &Infobuffer);
}




Engine::Reference<Light> STDGLLightSystem::MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float fov, vec3 color, float near, float far) {
    uint32_t ID;
    if (FreedIndices.empty()) {
        ID = NextIndexToMake++;
    } else {
        ID = FreedIndices.front();
        FreedIndices.pop();
    }

    auto* Resource = new Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>(this, this, RWorldRef, ID, Type, resolution, fov, color, near, far);
    LightResources[ID] = Resource;
    return Engine::Reference(Resource);
}