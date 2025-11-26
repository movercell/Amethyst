#pragma once
#include "master.h"
// Default camera values
const float CAMERA_DEFAULT_YAW         = -90.0f;
const float CAMERA_DEFAULT_PITCH       =  0.0f;
const float CAMERA_DEFAULT_SPEED       =  2.5f;
const float CAMERA_DEFAULT_SENSITIVITY =  0.1f;
const float CAMERA_DEFAULT_FOV         =  120.0f;

class Camera {
public:
    // camera Attributes
    vec3 Position;
    // euler Angles
    float Yaw   = CAMERA_DEFAULT_YAW;
    float Pitch = CAMERA_DEFAULT_PITCH;
    // camera options
    float MouseSensitivity = CAMERA_DEFAULT_SENSITIVITY;
    float FOV              = CAMERA_DEFAULT_FOV;
    glm::vec2 Resolution;
};