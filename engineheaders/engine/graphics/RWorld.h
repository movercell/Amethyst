#pragma once

#include "Camera.h"
#include "engine/graphics/ModelInstance.h"
#include <memory>

/*!
* \brief A world for rendering purposes.
*
*  Stores information about model instances, cameras, lights, etc.
*  Intended to be linked with an actual game world, so use it outside of one at your own risk.
*/
class RWorld {
public:
    //! Makes a camera.
    virtual std::shared_ptr<Camera> MakeCamera(vec2 resolution, const std::string& name, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH) = 0;
    //! Gets a camera.(used for getting the camera's render result in window UI functions)
    virtual Camera* GetCamera(std::string name) = 0;
    //! Adds a model instance to the scene.
    virtual std::shared_ptr<ModelInstance> MakeModelInstance() = 0;

    virtual ~RWorld() {};
};