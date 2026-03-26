#pragma once
#include <glad/glad.h>

struct DrawElementsIndirectCommand {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct alignas(16) DrawElementsIndirectCommandSTD140 {
    GLuint count = 0;
    GLuint instanceCount = 0;
    GLuint firstIndex = 0;
    GLuint baseVertex = 0;
    GLuint baseInstance = 0;
};