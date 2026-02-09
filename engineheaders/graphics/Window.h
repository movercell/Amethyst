#pragma once
#include "master.h"

#include <functional>
#include <memory>

#ifdef AMETHYSTENGINESRC
 #include "../../src/graphics/misc.h"
#endif

struct ____WindowData;
struct ____UIData;

class Renderer;

/*!
*   \brief A window.
*/
class ENGINEEXPORT Window {
protected:
    //!@private
    std::function<void(Renderer*, Window*)> UIFunction;
    //!@private
    std::shared_ptr<Renderer> rendererRef = nullptr;
    //!@private
    ____WindowData* rendererData = nullptr;
    //!@private
    ____UIData* UIData = nullptr;
    //!@private
    ____WindowData* data = nullptr;
    //!@private
    bool ShouldEatCursor = false;
    //!@private
    void ProcessCursorEating();
    //!@private
    Window() {}; //the compiler compains if this doesn't exist

    #ifdef AMETHYSTENGINESRC
        friend class GraphicsMisc;
    #endif
public:
    virtual ~Window();
    //! Re-creates the system window with the current parameters of this object.
    void Update();
    //! Sets the UI function
    void SetUIFunction(std::function<void(Renderer*, Window*)> Function);
    //! Consumes the cursor
    void EatCursor(bool state);

    std::string Name = "Unnamed window";
};
