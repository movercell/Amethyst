#include "STDGLCamera.h"
#include "GLFW/glfw3.h"
#include "glm/geometric.hpp"

void STDGLCamera::Bind() {
    if (wasChanged) {
        Info.View = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
        mat4 projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, Near, Far);
        Info.ViewProjection = projection * Info.View;

        Info.Frustum = Shapes::Frustum(Info.ViewProjection);
        Info.CameraPos = Position;
        glNamedBufferSubData(Infobuffer, 0, sizeof(Camerainfo_t), &Info);

        wasChanged = false;
    }
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Infobuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
}

void STDGLCamera::CreateBuffers() {
    glCreateFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glCreateTextures(GL_TEXTURE_2D, 2, AllTextureBuffers);
    // Color buffer
    glTextureStorage2D (Colorbuffer, 1, GL_RGBA8, Resolution.x, Resolution.y);
    glTextureParameteri(Colorbuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(Colorbuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glNamedFramebufferTexture(Framebuffer, GL_COLOR_ATTACHMENT0, Colorbuffer, 0);
    // Depth buffer
    glTextureStorage2D(Depthbuffer, 1, GL_DEPTH24_STENCIL8, Resolution.x, Resolution.y);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Depthbuffer, 0);
    glNamedFramebufferTexture(Framebuffer, GL_STENCIL_ATTACHMENT, Depthbuffer, 0);

    // Information buffer
    glCreateBuffers(1, &Infobuffer);
    glNamedBufferStorage(Infobuffer, sizeof(Camerainfo_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

uint32_t STDGLCamera::GetTexture() {
    return Colorbuffer;
}

uint32_t STDGLCamera::GetDepthTexture() {
    return Depthbuffer;
}

STDGLCamera::~STDGLCamera() {
    glfwMakeContextCurrent(Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glDeleteTextures(2, AllTextureBuffers);
    glDeleteBuffers(1, &Infobuffer);
}
