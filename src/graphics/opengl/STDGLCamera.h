#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

#include "engine/graphics/Camera.h"
#include "STDGLRenderer.h"
#include "engine/geometry/Frustum.h"

const vec3 WorldUp = vec3(0.0f, 0.0f, 1.0f);


struct STDGLCamera : public Camera {
    GLuint Framebuffer;
    union {
        struct {
            GLuint Colorbuffer, Depthbuffer, Normalbuffer;
        };
        GLuint AllTextureBuffers[3];
    };
    GLuint Infobuffer;
    GLFWwindow* Context;

    struct Camerainfo_t {
        mat4 View;
        mat4 Projection;
        mat4 ViewProjection;
        mat4 InverseView;
        mat4 InverseViewProjection;
        Shapes::Frustum Frustum;
        vec3 CameraPos;
    };
    Camerainfo_t Info;

    Engine::Reference<RWorld> RWorldRef;
    // Constructor with vectors.
    STDGLCamera(Engine::Reference<RWorld> rworldref, vec2 resolution, const std::string& name, float fov = CAMERA_DEFAULT_FOV, float near = CAMERA_DEFAULT_NEAR, float far = CAMERA_DEFAULT_FAR);

    // Updates the internal data of the camera.
    void Update();
    // Binds the camera into UBO slot 0.
    void Bind();

    uint32_t GetTexture();
    uint32_t GetDepthTexture();
    void SetPosition(vec3 Position);
    void SetRotation(quat Rotation);
    void SetFov(float fov);

    ~STDGLCamera();

private:
    // Creates the buffers.
    void CreateBuffers();
    
    bool wasChanged = true;
    bool wasChangedProjection = true;
};