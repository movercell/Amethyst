#pragma once

#include "Camera.h"

struct Light : public Engine::Internal::BaseCameraOrLight {
    void SetRotation(quat Rotation) {
        Engine::Internal::BaseCameraOrLight::SetRotation(Rotation);
        HandleChange();
    }
    void SetAngles(vec3 Angle) {
        SetRotation(quat(Angle));
    }
    void SetPosition(vec3 Pos) {
        Engine::Internal::BaseCameraOrLight::SetPosition(Pos);
        HandleChange();
    }
    void SetFOV(float fov) {
        FOV = fov;
        HandleChange();
    }
    void SetColor(vec3 color) {
        Color = color;
        HandleChange();
    }

    virtual ~Light() {};

protected:
    vec3 Color;
    virtual void HandleChange() = 0;
};