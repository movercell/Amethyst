#pragma once

#include "engine/master.h"

#include <functional>
#include <memory>

class Renderer;

/*!
*   \brief A window.
*/
class ENGINEEXPORT Window {
protected:
    //!@private
    std::function<void(Renderer*, Window*)> UIFunction;
    //!@private
    std::string Name;
public:
    virtual ~Window() {};
    //! Sets the UI function
    virtual void SetUIFunction(std::function<void(Renderer*, Window*)> Function) {
        UIFunction = Function;
    }
    //! Consumes the cursor
    virtual void SetEatCursor(bool state) = 0;
    //! Is the cursor being consumed?
    virtual bool IsEatingCursor() = 0;
    //! Is the window in focus?
    virtual bool IsWindowInFocus() = 0;
    //! Sets the name of the window;
    virtual void SetName(std::string name) = 0;
    //! Sets the resolution of the window.
    virtual void SetResolution(int x, int y) = 0;
    //! Gets the width of the window.
    virtual int GetWidth() = 0;
    //! Gets the height of the window.
    virtual int GetHeight() = 0;
};
