#pragma once
#include "rendering/Window.h"
#include "rendering/Context.h"

class GLContext : public Context
{
private:
    /* data */
public:
    ~GLContext();
    static std::shared_ptr<Context> Make();
    void Render();
};
