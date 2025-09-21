#pragma once

#include "rendering/Window.h"
#include <array>
#include <memory>

class Context
{
protected:

    class InternalWindow : Window {
        public:

        bool isValid = false;
        void Construct(WindowData* = nullptr);
        InternalWindow() {};
        ~InternalWindow() {isValid = false;}
        WindowData* GetData() {return data;}
    };

    //stored for making windows reference this their context(needs to be set _after_ making the first window)
    std::weak_ptr<Context> selfref;
    std::array<InternalWindow, 10> windowArray;
public:

    Context();
    virtual ~Context();
    //virtual void Render() = 0;
};


