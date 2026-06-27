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
    Data.ViewProjection = Info.ViewProjection;
    Data.Type = Type;
    Data.Color = Color;
    Data.Far = Far;
    Data.FOV = FOV;
    Data.Near = Near;
    Data.TextureSpace = TextureSpace;
    Data.LightPos = Position;
    glNamedBufferSubData(Owner->LightDataBuffer, sizeof(STDGLLightData) * ID, sizeof(STDGLLightData), &Data);
}

STDGLLight::~STDGLLight() {
    glfwMakeContextCurrent(Owner->Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glDeleteBuffers(1, &Infobuffer);

    // The block has padding that needs to be accounted for when freeing.
    TextureSpace.PosX -= STDGLLIGHT_ALLOC2D_PADDING;
    TextureSpace.PosY -= STDGLLIGHT_ALLOC2D_PADDING;
    TextureSpace.SizeX += STDGLLIGHT_ALLOC2D_PADDING * 2;
    TextureSpace.SizeY += STDGLLIGHT_ALLOC2D_PADDING * 2;
    Owner->LightDepthBufferAllocator.Free(TextureSpace);
}




Engine::Reference<Light> STDGLLightSystem::MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float fov, vec3 color, float near, float far) {
    uint32_t ID;
    if (FreedIndices.empty()) {
        ID = NextIndexToMake++;
    } else {
        ID = FreedIndices.front();
        FreedIndices.pop();
    }

    // Don't forget to pad the block to prevent texture bleeding artifacts!
    auto RawBlock = LightDepthBufferAllocator.Alloc(resolution.x + STDGLLIGHT_ALLOC2D_PADDING * 2, resolution.y + STDGLLIGHT_ALLOC2D_PADDING * 2);
    auto Block = Geometry::Alloc2D::Block(RawBlock.PosX + STDGLLIGHT_ALLOC2D_PADDING, RawBlock.PosY + STDGLLIGHT_ALLOC2D_PADDING, resolution.x, resolution.y);

    auto* Resource = new Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>(this, this, RWorldRef, ID, Type, Block, fov, color, near, far);
    LightResources[ID] = Resource;
    return Engine::Reference(Resource);
}