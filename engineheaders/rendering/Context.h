#pragma once

#include "rendering/Window.h"
#include <vector>
#include <memory>

class Context
{
protected:
    class ____Window : public Window {
    public:
        void Update();
        ____Window(std::weak_ptr<Context> context, ____WindowData* ContextData);
        ____WindowData* GetData() {return data;}
        void Draw();
    };
    
    ____WindowData* contextData = nullptr;
    std::weak_ptr<Context> selfRef; //stored for making windows reference this their context
    std::vector<std::weak_ptr<____Window>> windowVector = std::vector<std::weak_ptr<____Window>>();
    
public:
    virtual ~Context() {};
    std::shared_ptr<Window> MakeWindow();
    virtual void Render() = 0;
};


