#pragma once
#include "engine/graphics/Camera.h"
#include "engine/master.h"
#include "engine/Resource.h"

#include "engine/graphics/Window.h"
#include "engine/graphics/RWorld.h"
#include <string>
#include <vector>
#include <memory>



/*!
*  \brief The standard renderer interface, every renderer must conform to it.
*
*   Also provides the function for adding a window. \n
*   The renderer does ***not*** count as a reference to anything within it, hovewer, any resource reference that can be gotten from it ***will*** count as a reference to the renderer owning it
*/
class ENGINEEXPORT Renderer {
public:
    virtual ~Renderer() {}; //the compiler compains if this doesn't exist
    //! Makes a window.
    virtual Engine::Reference<Window> MakeWindow(int x = 800, int y = 600, std::string name = "Unnamed window") = 0;
    //! Makes a Render World.(Note: Supposed to only be used for game worlds, use separately at your own risk!)
    virtual Engine::Reference<RWorld> MakeRWorld() = 0;
    //! Gets a camera.(Used for getting the camera's render result in window UI functions.)
    virtual Camera* GetCamera(std::string name) = 0;
    //! Gets a constant reference to the internal frame counter
    virtual const uint64_t& GetFrameCounter() = 0;
    //! Draws everything.
    virtual void Draw() = 0;
    //! Makes a renderer with the passed in classname.(E.g. "STDGLRenderer")
    static Engine::Reference<Renderer> Make(std::string classname);

#ifdef AMETHYSTENGINESRC
    static void AddRenderer(const std::string classname, Engine::Reference<Renderer> (*makefunc)() );
#endif

};


