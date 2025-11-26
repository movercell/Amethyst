#pragma once

#define ENGINEEXPORT __attribute__ ((visibility ("default")))

struct vec3 {
    float x = 0, y = 0, z = 0;
    vec3(float X, float Y, float Z) { x = X; y = Y; z = Z; }
    vec3() {}

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec3(const glm::vec3& other) : x(other.x), y(other.y), z(other.z) {}
    glm::vec3 toglm(){ return glm::vec3(x, y, z); }
#endif
};

struct vec2 {
    float x = 0, y = 0;
    vec2(float X, float Y) { x = X; y = Y; }
    vec2() {}

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec2(const glm::vec2& other) : x(other.x), y(other.y) {}
    glm::vec2 toglm(){ return glm::vec2(x, y); }
#endif
};

struct vec4 {
    float x = 0, y = 0, z = 0, w =0;
    vec4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; }
    vec4() {}

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec4(const glm::vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    glm::vec4 toglm(){ return glm::vec4(x, y, z, w); }
#endif
};
