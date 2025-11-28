#include "STDGLCamera.h"

void STDGLCamera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.z = sin(glm::radians(Pitch));
    front.y = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(Front.cross(WorldUp).toglm());  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(Right.cross(Front).toglm());
}

void STDGLCamera::Bind(GLuint CameraMatrixBuffer) {
    glm::mat4 view = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
    glm::mat4 projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, 0.1f, 100.0f);
    glm::mat4 viewprojection = projection * view;
    glNamedBufferData(CameraMatrixBuffer, sizeof(glm::mat4), &viewprojection, GL_DYNAMIC_DRAW);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
}

void STDGLCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw   += xoffset;
    Pitch -= yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
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
    // Color buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &Colorbuffer);
    glTextureStorage2D(Colorbuffer, 1, GL_RGB8, Resolution.x, Resolution.y);
    glTextureParameteri(Colorbuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(Colorbuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glNamedFramebufferTexture(Framebuffer, GL_COLOR_ATTACHMENT0, Colorbuffer, 0);
    // Depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &Depthbuffer);
    glTextureStorage2D(Depthbuffer, 1, GL_DEPTH_COMPONENT24, Resolution.x, Resolution.y);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(Depthbuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Depthbuffer, 0);
}
