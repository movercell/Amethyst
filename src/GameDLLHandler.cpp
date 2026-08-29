#include "GameDLLHandler.h"
#include "engine/master.h"
#include <string>
#include <dlfcn.h>

#ifdef _WIN32

// TODO: Make a windows version.(And also give it a message of possible compiler mismatch.)

#else

GameDLL::GameDLL() {
    // TODO: Load from game path, not working directory.
	dllhandle = ::dlopen("./libgame.so", RTLD_NOW);
    if (!dllhandle) {
        Engine::Error(std::string("Could not open game library: ") + ::dlerror());
    }

	gameinit_proxy = (gameinit_signature)dlsym(dllhandle, "gameinit");
	const char* dlsym_error = ::dlerror();
    if (dlsym_error) {
        Engine::Error(std::string("Could not locate the game library's `gameinit` symbol: ") + ::dlerror());
    }

	gameloop_proxy = (gameloop_signature)dlsym(dllhandle, "gameloop");
	dlsym_error = ::dlerror();
    if (dlsym_error) {
        Engine::Error(std::string("Could not locate the game library's `gameloop` symbol: ") + ::dlerror());
    }
}

GameDLL::~GameDLL() {
    dlclose(dllhandle);
}

#endif