#pragma once

class GameDLL {
    using gameinit_signature = void(*)();
    using gameloop_signature = void(*)();
    gameinit_signature gameinit_proxy;
    gameloop_signature gameloop_proxy;

#ifdef _WIN32
    // TODO: Make a windows version.
#else
    void* dllhandle;
#endif

public:
    GameDLL();
    ~GameDLL();

    inline void init() { gameinit_proxy(); }
    inline void loop() { gameloop_proxy(); }
};