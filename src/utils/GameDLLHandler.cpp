#include "GameDLLHandler.h"
#include "engine/master.h"
#include "engine/filesystem/Filesystem.h"
#include <string>

#ifdef _WIN32

GameDLL::GameDLL() {
    dllhandle = ::LoadLibrary((Filesystem::GetGameDirectoryPath()/"libgame.dll").c_str());

    if (!dllhandle) {
        Engine::Error(std::string("Could not open game library: ") + ::GetLastError());
    }

	gameinit_proxy = (gameinit_signature)GetProcAddress(dllhandle, "gameinit");
    if (gameinit_proxy == nullptr) {
        Engine::Error(std::string("Could not locate the game library's `gameinit` symbol(Possible compiler mismatch?): ") + ::GetLastError());
    }

	gameloop_proxy = (gameloop_signature)GetProcAddress(dllhandle, "gameloop");
    if (gameloop_proxy == nullptr) {
        Engine::Error(std::string("Could not locate the game library's `gameloop` symbol(Possible compiler mismatch?): ") + ::GetLastError());
    }
}

#else

GameDLL::GameDLL() {
	dllhandle = ::dlopen((Filesystem::GetGameDirectoryPath()/"libgame.so").c_str(), RTLD_NOW);
    if (!dllhandle) {
        Engine::Error(std::string("Could not open game library: ") + ::dlerror());
    }

	gameinit_proxy = (gameinit_signature)dlsym(dllhandle, "gameinit");
    if (gameinit_proxy == nullptr) {
        Engine::Error(std::string("Could not locate the game library's `gameinit` symbol: ") + ::dlerror());
    }

	gameloop_proxy = (gameloop_signature)dlsym(dllhandle, "gameloop");
    if (gameloop_proxy == nullptr) {
        Engine::Error(std::string("Could not locate the game library's `gameloop` symbol: ") + ::dlerror());
    }
}

#endif