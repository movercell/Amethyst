#include "engine/master.h"
#include <iostream>
#include <cstdlib>

void Engine::Warning(const std::string& text) {
    std::cout << "This process issued the following warning:\n\n" << text << std::endl;
}
void Engine::Print(const std::string& text) {
    std::cout << text << std::endl;
}

void Engine::Error(const std::string& text) {
    std::cout << "This process encountered an unrecoverable error!\n\n" << text << std::endl;
    std::abort();
}