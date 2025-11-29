#pragma once

#include "STDGLCamera.h"
#include "graphics/RWorld.h"
#include "graphics/opengl/STDGLRenderer.h"
#include <memory>
#include <vector>

class STDGLRWorld : public RWorld {
public:
    std::shared_ptr<Camera> MakeCamera(vec2 resolution, const std::string& name, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH);

    ~STDGLRWorld();
    STDGLRWorld(std::weak_ptr<Renderer> Renderer) {
        renderer = std::static_pointer_cast<STDGLRenderer>(Renderer.lock());
    }

    std::vector<std::weak_ptr<STDGLCamera>> CameraVec;
    std::shared_ptr<STDGLRenderer> renderer;
};