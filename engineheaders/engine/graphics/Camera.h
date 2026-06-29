#pragma once
#include "engine/master.h"
#include <cstdint>
#include <string>
// Default camera values
inline constexpr float CAMERA_DEFAULT_YAW         =  0.0f;
inline constexpr float CAMERA_DEFAULT_PITCH       =  0.0f;
inline constexpr float CAMERA_DEFAULT_SPEED       =  2.5f;
inline constexpr float CAMERA_DEFAULT_SENSITIVITY =  0.1f;
inline constexpr float CAMERA_DEFAULT_FOV         =  120.0f;
inline constexpr float CAMERA_DEFAULT_NEAR        =  1.0f;
inline constexpr float CAMERA_DEFAULT_FAR         =  32768.0f;

namespace Engine {
    //@internal
    namespace Internal {
        struct BaseCameraOrLight {
            void SetRotation(quat Rotation) {
                mat4 RotationMatrix = quat(Rotation).MakeRotationMatrix();

                Front = RotationMatrix[0].ToVec3();
                Left = RotationMatrix[1].ToVec3();
                Up = RotationMatrix[2].ToVec3();

                wasChanged = true;
            }
            void SetAngles(vec3 Angle) {
                SetRotation(quat(Angle));
            }

            void SetPosition(vec3 Pos) {
                Position = Pos;

                wasChanged = true;
            }

            inline vec2 GetResolution() { return Resolution; };
        protected:
            vec3 Position;

            vec3 Front;
            vec3 Left;
            vec3 Up;

            float FOV = CAMERA_DEFAULT_FOV;
            float Near = CAMERA_DEFAULT_NEAR;
            float Far = CAMERA_DEFAULT_FAR;
            vec2 Resolution;

            bool wasChanged = true;
        };
    }
}
/*!
* \brief A camera interface.
*/
struct Camera : public Engine::Internal::BaseCameraOrLight {
    //! Name of the camera by which it can be queried
    std::string Name;
    
    //! Gets the coordinate front axis.
    vec3 GetFront() { return Front; };
    //! Gets the coordinate left axis.
    vec3 GetLeft() { return Left; };
    //! Gets the coordinate up axis.
    vec3 GetUp() { return Up; };

    virtual uint32_t GetTexture() = 0;
    virtual uint32_t GetDepthTexture() = 0;

    virtual ~Camera() {};

};