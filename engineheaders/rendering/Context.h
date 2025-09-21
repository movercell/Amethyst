#pragma once

#include "Window.h"
#include <array>

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

    std::array<InternalWindow, 10> windowArray;
public:

    Context();
    virtual ~Context();
    //virtual void Render() = 0;
};


