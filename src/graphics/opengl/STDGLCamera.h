#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "graphics/Camera.h"

const vec3 WorldUp = vec3(0.0f, 0.0f, -1.0f);


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class STDGLCamera : public Camera {
public:

    GLuint Framebuffer;
    GLuint Colorbuffer;
    GLuint Depthbuffer;

    // Constructor with vectors.
    STDGLCamera(vec2 resolution, const std::string& name, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH) {
        Resolution = resolution;
        Name = name;
        Position = position;
        Yaw = yaw;
        Pitch = pitch;
        CreateBuffers();

        UpdateCameraVectors();
    }

    // Binds the camera into UBO slot 0.
    void Bind(GLuint CameraMatrixBuffer);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    uint32_t GetTexture();
    uint32_t GetDepthTexture();

private:
    // Creates the buffers.
    void CreateBuffers();
    // Calculates the front vector from the Camera's (updated) Euler Angles.
    void UpdateCameraVectors();
};