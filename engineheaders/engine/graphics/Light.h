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

    virtual ~Light() = 0;

protected:
    virtual void HandleChange() = 0;
};