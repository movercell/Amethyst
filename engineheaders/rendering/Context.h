#pragma once

#include "rendering/Window.h"
#include <vector>
#include <memory>



/*!
*  \brief The standard graphics context interface, every graphics context must conform to it.
*
*   Also provides the function for adding a window. \n
*   The context does ***not*** count as a reference to anything within it except it's asset manager.
*/
class Context
{
protected:
    //!@private
    class ____Window : public Window {
    public:
        void Update();
        ____Window(std::weak_ptr<Context> context, ____WindowData* ContextData);
        ____WindowData* GetData() {return data;}
        void Draw();
    };

    //!@private
    ____WindowData* contextData = nullptr;
    //!@private
    std::weak_ptr<Context> selfRef; //stored for making windows reference this their context
    //!@private
    std::vector<std::weak_ptr<____Window>> windowVector =
                            std::vector<std::weak_ptr<____Window>>();
    
public:
    virtual ~Context() {}; //the compiler compains if this doesn't exist
    //!Makes a window
    std::shared_ptr<Window> MakeWindow();
    //!Draws the window UIs.
    virtual void Draw() = 0;
};


