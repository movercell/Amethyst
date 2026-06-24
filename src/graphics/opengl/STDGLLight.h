#pragma once

#include "STDGLCamera.h"
#include "engine/graphics/Light.h"

struct STDGLLight : public Light {
    GLuint Framebuffer;
    GLuint Depthbuffer;
    GLuint Infobuffer;
    GLuint Depthhandle;
    GLFWwindow* Context;
    STDGLCamera::Camerainfo_t Info;
    uint32_t ID;


    STDGLLight(GLFWwindow* context, uint32_t id, vec2 resolution, float fov, float near, float far) {
        Context = context;
        ID = id;
        Resolution = resolution;
        FOV = fov;
        Near = near;
        Far = far;
        CreateBuffers();
    }
    ~STDGLLight();

    void Bind(bool wasChanged);

    GLuint GetHandle() {
        return Depthhandle;
    }
protected:
    void HandleChange();

    void CreateBuffers();
};