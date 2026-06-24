#include "STDGLLight.h"

void STDGLLight::Bind(bool wasChanged) {
    if (wasChanged) {
        Info.View = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
        mat4 projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, Near, Far);
        Info.ViewProjection = projection * Info.View;

        Info.Frustum = Shapes::Frustum(Info.ViewProjection);
        Info.CameraPos = Position;
        glNamedBufferSubData(Infobuffer, 0, sizeof(STDGLCamera::Camerainfo_t), &Info);
    }
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Infobuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
}

void STDGLLight::CreateBuffers() {
    glCreateFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    // Depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &Depthbuffer);
    glTextureStorage2D(Depthbuffer, 1, GL_DEPTH24_STENCIL8, Resolution.x, Resolution.y);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Depthbuffer, 0);
    glNamedFramebufferTexture(Framebuffer, GL_STENCIL_ATTACHMENT, Depthbuffer, 0);

    Depthhandle = glGetTextureHandleARB(Depthbuffer);
    glMakeTextureHandleResidentARB(Depthhandle);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Information buffer
    glCreateBuffers(1, &Infobuffer);
    glNamedBufferStorage(Infobuffer, sizeof(STDGLCamera::Camerainfo_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void STDGLLight::HandleChange() {
    // oof
}

STDGLLight::~STDGLLight() {
    glfwMakeContextCurrent(Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glMakeTextureHandleNonResidentARB(Depthhandle);
    glDeleteTextures(1, &Depthbuffer);
    glDeleteBuffers(1, &Infobuffer);
}