#pragma once
#include "engine/graphics/Camera.h"
#include "engine/master.h"
#include "engine/weak_vector.h"

#include "engine/graphics/Window.h"
#include "engine/graphics/RWorld.h"
#include <string>
#include <vector>
#include <memory>



/*!
*  \brief The standard renderer interface, every renderer must conform to it.
*
*   Also provides the function for adding a window. \n
*   The renderer does ***not*** count as a reference to anything within it.
*/
class ENGINEEXPORT Renderer {
protected:
    //!@private
    std::weak_ptr<Renderer> selfRef; // Stored for making windows reference this their context
    
public:
    virtual ~Renderer() {}; //the compiler compains if this doesn't exist
    //! Makes a window.
    virtual std::shared_ptr<Window> MakeWindow() = 0;
    //! Makes a Render World.(Note: Supposed to only be used for game worlds, use separately at your own risk!)
    virtual std::shared_ptr<RWorld> MakeRWorld() = 0;
    //! Gets a camera.(used for getting the camera's render result in window UI functions)
    virtual Camera* GetCamera(std::string name) = 0;
    //! Draws everything.
    virtual void Draw() = 0;
};


