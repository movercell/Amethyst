#pragma once
#include "master.h"

#include <memory>

struct ____WindowData;

class Context;

/*!
*   \brief A window.
*/
class EXPORT Window {
public:
    virtual ~Window();
    //! Re-creates the system window with the current parameters of this object.
    void Update();

protected:
    //!@private
    std::shared_ptr<Context> contextRef = nullptr;
    //!@private
    ____WindowData* contextData = nullptr;
    //!@private
    ____WindowData* data = nullptr;    
    //!@private
    Window() {}; //the compiler compains if this doesn't exist
};
