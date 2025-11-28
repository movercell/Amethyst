#include "STDGLCamera.h"
#include "graphics/RWorld.h"
#include <memory>
#include <vector>

class STDGLRWorld : RWorld {
public:
    std::shared_ptr<Camera> MakeCamera(vec2 resolution, vec3 position = vec3(0.0f, 0.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH);

    ~STDGLRWorld();
private:
    std::vector<std::weak_ptr<STDGLCamera>> CameraVec;
};