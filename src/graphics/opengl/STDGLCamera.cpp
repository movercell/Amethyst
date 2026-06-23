#include "STDGLCamera.h"
#include "GLFW/glfw3.h"
#include "glm/geometric.hpp"

void STDGLCamera::UpdateCameraVectors() {
    mat4 Rotation = quat(vec3(Pitch, Yaw, 0.0f)).MakeRotationMatrix();

    Front = Rotation[0].ToVec3();
    Left = Rotation[1].ToVec3();
    Up = Rotation[2].ToVec3();
}

void STDGLCamera::Bind() {
    Info.View = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
    mat4 projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, CAMERA_DEFAULT_NEAR, CAMERA_DEFAULT_FAR);
    Info.ViewProjection = projection * Info.View;

    Info.Frustum = Shapes::Frustum(Info.ViewProjection);
    Info.CameraPos = Position;
    glNamedBufferSubData(Infobuffer, 0, sizeof(Camerainfo_t), &Info);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Infobuffer);
    glNamedFramebufferTexture(Framebuffer, GL_COLOR_ATTACHMENT0, Colorbuffers[*FrameCounterPtr & 1], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
}

void STDGLCamera::ProcessMouseMovement(vec2 offset, bool constrainPitch)
{
    offset.x *= MouseSensitivity;
    offset.y *= MouseSensitivity;
    Yaw   -= offset.x;
    Pitch += offset.y;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}


void STDGLCamera::CreateBuffers() {
    glCreateFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    // Color buffers
    glCreateTextures(GL_TEXTURE_2D, 2, Colorbuffers);
    for (int buffer = 0; buffer < 2; buffer++) {
        glTextureStorage2D(Colorbuffers[buffer], 1, GL_RGB8, Resolution.x, Resolution.y);
        glTextureParameteri(Colorbuffers[buffer], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(Colorbuffers[buffer], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // Depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &Depthbuffer);
    glTextureStorage2D(Depthbuffer, 1, GL_DEPTH_COMPONENT24, Resolution.x, Resolution.y);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Depthbuffer, 0);

    // Information buffer
    glCreateBuffers(1, &Infobuffer);
    glNamedBufferStorage(Infobuffer, sizeof(Camerainfo_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

uint32_t STDGLCamera::GetTexture() {
    return Colorbuffers[*FrameCounterPtr & 1];
}

uint32_t STDGLCamera::GetDepthTexture() {
    return Depthbuffer;
}

STDGLCamera::~STDGLCamera() {
    glfwMakeContextCurrent(Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glDeleteTextures(3, Colorbuffers); // Also removes the depth buffer.
    glDeleteBuffers(1, &Infobuffer);
}
