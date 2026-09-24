#pragma once

#include <string>
#include "master.h"

namespace Engine {
    //! Handles simple C-style escape sequences.
    char CharacterEscapeResult(char original) {
        switch (original)
        {
        case '\'':
            return 0x27;
        case '\"':
            return 0x22;
        case '?':
            return 0x3f;
        case '\\':
            return 0x5c;
        case 'a':
            return 0x07;
        case 'b':
            return 0x08;
        case 'f':
            return 0x0c;
        case 'n':
            return 0x0a;
        case 'r':
            return 0x0d;
        case 't':
            return 0x09;
        case 'v':
            return 0x0b;
        case '0':
            Engine::Warning("Engine::CharacterEscapeResult: Attempted to escape a null terminator. Are you sure about doing that?");
            return '\v';
        }

        Engine::Warning(std::string("Engine::CharacterEscapeResult: Attemped to use a bad escape character: ") + original);
        return '\v';
    }
}