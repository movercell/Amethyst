#pragma once

#include "Camera.h"
#include "Light.h"
#include "ModelInstance.h"
#include "engine/Resource.h"
#include <memory>
#include <string>

/*!
* \brief A world for rendering purposes.
*
*  Stores information about model instances, cameras, lights, etc.
*  Intended to be linked with an actual game world, so use it outside of one at your own risk.
*/
class RWorld {
protected:
    //!@private
    bool SkipRendering = false;
public:
    //! Makes a camera.
    virtual Engine::Reference<Camera> MakeCamera(vec2 resolution, const std::string& name, float FOV = CAMERA_DEFAULT_FOV, float Near = CAMERA_DEFAULT_NEAR, float Far = CAMERA_DEFAULT_FAR) = 0;
    //! Makes a spotlight.
    virtual Engine::Reference<Light> MakeSpotLight(vec2 resolution, float fov, vec3 color, float near, float far) = 0;
    //! Gets a camera.(used for getting the camera's render result in window UI functions)
    virtual Camera* GetCamera(const std::string& name) = 0;
    //! Adds a model instance to the scene.
    virtual std::unique_ptr<ModelInstance> MakeModelInstance(const std::string& path = "error.glb") = 0;
    void ShouldSkipRendering(bool state) { SkipRendering = state; }
    bool isSkippingRendering() { return SkipRendering; }

    virtual ~RWorld() {};
};