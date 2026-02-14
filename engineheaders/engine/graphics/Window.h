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
public:
    virtual ~Window() {};
    //! Re-creates the system window with the current parameters of this object.
    virtual void Update() = 0;
    //! Sets the UI function
    virtual void SetUIFunction(std::function<void(Renderer*, Window*)> Function) {
        UIFunction = Function;
    }
    //! Consumes the cursor
    virtual void EatCursor(bool state) = 0;
    //! Is the cursor being consumed?
    virtual bool IsEatingCursor() = 0;
    //! Is the window in focus?
    virtual bool IsWindowInFocus() = 0;

    //! Window name.
    std::string Name = "Unnamed window";
    //! Window resolution
    vec2 Resolution = vec2(800, 600);
};
