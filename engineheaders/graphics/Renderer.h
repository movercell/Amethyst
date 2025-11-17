#pragma once
#include "master.h"

#include "graphics/Window.h"
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
    std::weak_ptr<Renderer> selfRef; //stored for making windows reference this their context
    //!@private
    std::vector<std::weak_ptr<____Window>> windowVector =
                            std::vector<std::weak_ptr<____Window>>();
    //!@private
    virtual void UINewFrame() = 0;
    //!@private
    virtual void UIEndFrame() = 0; 
    //!@private
    virtual ____UIData* UINewData(____WindowData* window) = 0;
    
public:
    virtual ~Renderer() {}; //the compiler compains if this doesn't exist
    //!Makes a window
    std::shared_ptr<Window> MakeWindow();
    //!Draws everything.
    virtual void Draw() = 0;
};


