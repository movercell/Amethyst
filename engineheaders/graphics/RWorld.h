#include "Camera.h"
#include <memory>

/*!
* \brief A world for rendering purposes.
*
*  Stores information about model instances, cameras, lights, etc.
*  Intended to be linked with an actual game world, so use it outside of one at your own risk.
*/
class RWorld {
public:
    //!Makes a camera.
    virtual std::shared_ptr<Camera> MakeCamera(vec3 Position) = 0;

    virtual ~RWorld() {};
};