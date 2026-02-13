#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

#include "engine/graphics/Camera.h"
#include "engine/graphics/opengl/STDGLRenderer.h"

const vec3 WorldUp = vec3(0.0f, 0.0f, 1.0f);


class STDGLCamera : public Camera {
public:

    GLuint Framebuffer;
    GLuint Colorbuffer, Depthbuffer;
    GLuint Infobuffer;
    GLFWwindow* context;

    struct Camerainfo_t {
        mat4 ViewProjection;
    };
    Camerainfo_t Info;


    // Constructor with vectors.
    STDGLCamera(GLFWwindow* Context, vec2 resolution, const std::string& name, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH) {
        context = Context;
        Resolution = resolution;
        Name = name;
        Position = position;
        Yaw = yaw;
        Pitch = pitch;
        CreateBuffers();

        UpdateCameraVectors();
    }

    // Binds the camera into UBO slot 0.
    void Bind();

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(vec2 offset, bool constrainPitch = true);

    uint32_t GetTexture();
    uint32_t GetDepthTexture();

    ~STDGLCamera();

private:
    // Creates the buffers.
    void CreateBuffers();
    // Calculates the front vector from the Camera's (updated) Euler Angles.
    void UpdateCameraVectors();
};