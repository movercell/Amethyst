#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Camera.h"

const glm::vec3 WorldUp = glm::vec3(0.0f, 0.0f, -1.0f);


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class STDGLCamera : public Camera {
public:

    GLuint Framebuffer;

    // Constructor with vectors
    STDGLCamera(glm::vec2 resolution, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH) : Front(glm::vec3(1.0f, 0.0f, 0.0f)) {
        Resolution = resolution;
        Position = position;
        Yaw = yaw;
        Pitch = pitch;
        glCreateFramebuffers(1, &Framebuffer);
        UpdateCameraVectors();
    }
    // Constructor with scalar values
    STDGLCamera(float posX, float posY, float posZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)) {
        Position = vec3(posX, posY, posZ);
        Yaw = yaw;
        Pitch = pitch;
        glCreateFramebuffers(1, &Framebuffer);
        UpdateCameraVectors();
    }

    // Binds the camera into UBO slot 0
    void Bind(GLuint CameraMatrixBuffer);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
};