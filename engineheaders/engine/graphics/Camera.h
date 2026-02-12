#pragma once
#include "engine/master.h"
#include <cstdint>
#include <string>
// Default camera values
const float CAMERA_DEFAULT_YAW         =  0.0f;
const float CAMERA_DEFAULT_PITCH       =  0.0f;
const float CAMERA_DEFAULT_SPEED       =  2.5f;
const float CAMERA_DEFAULT_SENSITIVITY =  0.1f;
const float CAMERA_DEFAULT_FOV         =  120.0f;
const float CAMERA_DEFAULT_NEAR        =  1.0f;
const float CAMERA_DEFAULT_FAR         =  32768.0f;

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
    float MouseSensitivity = CAMERA_DEFAULT_SENSITIVITY;
    
    //! Coordinate front axis.
    vec3 Front = vec3(1, 0, 0);
    //! Coordinate up axis.
    vec3 Up = vec3(0, 0, 1);
    //! Coordinate right axis.
    vec3 Right = vec3(0, 1, 0);

    //! Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    virtual void ProcessMouseMovement(vec2 offset, bool constrainPitch = true) = 0;

    virtual uint32_t GetTexture() = 0;
    virtual uint32_t GetDepthTexture() = 0;

    virtual ~Camera() {};


    inline vec2 GetResolution() {return Resolution; };
protected:

    float FOV = CAMERA_DEFAULT_FOV;

    vec2 Resolution;

    struct Frustum
    {
        struct Plane{
            // unit vector
            vec3 Normal = vec3(0.f, 1.f, 0.f );
            // distance from origin to the nearest point in the plane
            float Distance = 0.f;    
            
            Plane() = default;

	        Plane(const vec3& p1, const vec3& norm)
	        	: Normal(norm.norm()),
	        	Distance(Normal.dot(p1)) {}

        };
    
        Plane TopFace;
        Plane BottomFace;

        Plane RightFace;
        Plane LeftFace;

        Plane FarFace;
        Plane NearFace;
    };

    Frustum CreateFrustum() {
        Frustum     frustum;
        float halfVSide = CAMERA_DEFAULT_FAR * tanf(FOV * .5f);
        float halfHSide = halfVSide * Resolution.x / Resolution.y;
        vec3 frontMultFar = Front * CAMERA_DEFAULT_FAR;

        frustum.NearFace = { Position + Front * CAMERA_DEFAULT_NEAR, Front };
        frustum.FarFace = { Position + frontMultFar, Front * -1 };
        frustum.RightFace = { Position,
                                (frontMultFar - Right * halfHSide).cross(Up) };
        frustum.LeftFace = { Position,
                                Up.cross(frontMultFar + Right * halfHSide) };
        frustum.TopFace = { Position,
                                Right.cross(frontMultFar - Up * halfVSide) };
        frustum.BottomFace = { Position,
                                (frontMultFar + Up * halfVSide).cross(Right) };

        return frustum;
    }
    
};