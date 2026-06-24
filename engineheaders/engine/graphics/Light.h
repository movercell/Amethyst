#pragma once

#include "Camera.h"

struct Light : public Engine::Internal::BaseCameraOrLight {
    void SetColor(vec3 color) {
        Color = color;
        wasChanged = true;
    }

    virtual ~Light() {};

protected:
    vec3 Color;
};