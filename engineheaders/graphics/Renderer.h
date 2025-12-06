#pragma once
#include "graphics/Camera.h"
#include "master.h"
#include "weak_vector.h"

#include "graphics/Window.h"
#include "graphics/RWorld.h"
#include <string>
#include <vector>
#include <memory>



/*!
*  \brief The standard renderer interface, every renderer must conform to it.
*
*   Also provides the function for adding a window. \n
*   The renderer does ***not*** count as a reference to anything within it.
*/
class ENGINEEXPORT Renderer
{
protected:
    //!@private
    class ____Window : public Window {
    public:
        ____Window(std::weak_ptr<Renderer> renderer, ____WindowData* RendererData);
        ____WindowData* GetData() {return data;}
        void Draw();
    };

    friend class Window;

    //!@private
    ____WindowData* rendererData = nullptr;
    //!@private
    std::weak_ptr<Renderer> selfRef; // Stored for making windows reference this their context
    //!@private
    weak_vector<____Window> WindowVector = weak_vector<____Window>();
    //!@private
    virtual void UINewFrame() = 0;
    //!@private
    virtual void UIEndFrame() = 0; 
    //!@private
    virtual ____UIData* UINewData(____WindowData* window) = 0;
    
public:
    virtual ~Renderer() {}; //the compiler compains if this doesn't exist
    //! Makes a window.
    std::shared_ptr<Window> MakeWindow();
    //! Makes a Render World.(Note: Supposed to only be used for game worlds, use separately at your own risk!)
    virtual RWorld* newRWorld() = 0;
    //! Deletes a Render World.
    virtual void deleteRWorld(RWorld* target) = 0;
    //! Gets a camera.(used for getting the camera's render result in window UI functions)
    virtual Camera* GetCamera(std::string name) = 0;
    //! Draws everything.
    virtual void Draw() = 0;
};


