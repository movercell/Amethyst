#pragma once
#include "master.h"
#include <cstdint>
#include <string>
// Default camera values
const float CAMERA_DEFAULT_YAW         = -90.0f;
const float CAMERA_DEFAULT_PITCH       =  0.0f;
const float CAMERA_DEFAULT_SPEED       =  2.5f;
const float CAMERA_DEFAULT_SENSITIVITY =  0.1f;
const float CAMERA_DEFAULT_FOV         =  120.0f;

/*!
* \brief A camera interface.
*/
class Camera {
public:
    //! Name of the camera by which it can be queried
    std::string Name; 

    // Camera attributes.
    vec3 Position;
    // Euler angles.
    float Yaw   = CAMERA_DEFAULT_YAW;
    float Pitch = CAMERA_DEFAULT_PITCH;
    // Camera options
    float MouseSensitivity = CAMERA_DEFAULT_SENSITIVITY;
    float FOV              = CAMERA_DEFAULT_FOV;
    vec2 Resolution;
    //! Coordinate front axis.
    vec3 Front = vec3(1.0f, 0.0f, 0.0f);
    //! Coordinate up axis.
    vec3 Up;
    //! Coordinate right axis.
    vec3 Right;

    //! Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    virtual void ProcessMouseMovement(vec2 offset, bool constrainPitch = true) = 0;

    virtual uint32_t GetTexture() = 0;
    virtual uint32_t GetDepthTexture() = 0;
};