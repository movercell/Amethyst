#pragma once

#include "Camera.h"

struct Light : public Engine::Internal::BaseCameraOrLight {
    virtual void SetPosition(vec3 Position) = 0;
    virtual void SetRotation(quat Rotation) = 0;
    inline void SetAngles(vec3 Angle) {
        SetRotation(quat(Angle));
    }

    virtual void SetColor(vec3 color) = 0;
    virtual void SetInnerAngle(float Inner) = 0;
    virtual void SetOuterAngle(float Outer) = 0;

    inline vec2 GetResolution() { return Resolution; };

    virtual ~Light() {};

protected:
    vec3 Color;
};