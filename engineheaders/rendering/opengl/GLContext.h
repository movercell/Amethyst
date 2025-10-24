#pragma once
#include "master.h"

#include "rendering/Window.h"
#include "rendering/Context.h"

/*!
*   \brief An OpenGL graphics context.
*/
class EXPORT GLContext : public Context
{
private:
    /* data */
public:
    ~GLContext();
    //!Makes the context
    static std::shared_ptr<Context> Make();
    void Draw();
};
