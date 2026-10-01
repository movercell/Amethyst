#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

class GameDLL {
    using gameinit_signature = void(*)();
    using gameloop_signature = void(*)();
    gameinit_signature gameinit_proxy;
    gameloop_signature gameloop_proxy;

#ifdef _WIN32
    HMODULE dllhandle;
#else
    void* dllhandle;
#endif

public:
    GameDLL();
    //~GameDLL(); <- No destructor should be there because the engine will still be filled with GameDLL virtual pointers after the end of the object's scope

    inline void init() { gameinit_proxy(); }
    inline void loop() { gameloop_proxy(); }
};