#pragma once

#ifdef GLMPresent // For syntax highlighting
    #include <glm/glm.hpp>
#endif

#define ENGINEEXPORT __attribute__ ((visibility ("default")))

struct vec3 {
    float x = 0, y = 0, z = 0;
    vec3(float X, float Y, float Z) { x = X; y = Y; z = Z; }
    vec3() {}
    vec3 operator+(const vec3& other) { return vec3( x + other.x, y + other.y, z + other.z); }
    vec3 operator-(const vec3& other) { return vec3( x - other.x, y - other.y, z - other.z); }
    vec3 operator*(const vec3& other) { return vec3( x * other.x, y * other.y, z * other.z); }
    vec3 operator/(const vec3& other) { return vec3( x / other.x, y / other.y, z / other.z); }

    vec3& operator+=(const vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    vec3& operator-=(const vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    vec3& operator*=(const vec3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
    vec3& operator/=(const vec3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }

    vec3 operator+(const float& other) { return vec3( x + other, y + other, z + other); }
    vec3 operator-(const float& other) { return vec3( x - other, y - other, z - other); }
    vec3 operator*(const float& other) { return vec3( x * other, y * other, z * other); }
    vec3 operator/(const float& other) { return vec3( x / other, y / other, z / other); }

    vec3& operator+=(const float& other) { x += other; y += other; z += other; return *this; }
    vec3& operator-=(const float& other) { x -= other; y -= other; z -= other; return *this; }
    vec3& operator*=(const float& other) { x *= other; y *= other; z *= other; return *this; }
    vec3& operator/=(const float& other) { x /= other; y /= other; z /= other; return *this; }
    
#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec3(const glm::vec3& other) : x(other.x), y(other.y), z(other.z) {}
    glm::vec3 toglm(){ return glm::vec3(x, y, z); }
#endif
};

struct vec2 {
    float x = 0, y = 0;
    vec2(float X, float Y) { x = X; y = Y; }
    vec2() {}
    vec2 operator+(const vec2& other) { return vec2( x + other.x, y + other.y); }
    vec2 operator-(const vec2& other) { return vec2( x - other.x, y - other.y); }
    vec2 operator*(const vec2& other) { return vec2( x * other.x, y * other.y); }
    vec2 operator/(const vec2& other) { return vec2( x / other.x, y / other.y); }

    vec2& operator+=(const vec2& other) { x += other.x; y += other.y; return *this; }
    vec2& operator-=(const vec2& other) { x -= other.x; y -= other.y; return *this; }
    vec2& operator*=(const vec2& other) { x *= other.x; y *= other.y; return *this; }
    vec2& operator/=(const vec2& other) { x /= other.x; y /= other.y; return *this; }

    vec2 operator+(const float& other) { return vec2( x + other, y + other); }
    vec2 operator-(const float& other) { return vec2( x - other, y - other); }
    vec2 operator*(const float& other) { return vec2( x * other, y * other); }
    vec2 operator/(const float& other) { return vec2( x / other, y / other); }

    vec2& operator+=(const float& other) { x += other; y += other; return *this; }
    vec2& operator-=(const float& other) { x -= other; y -= other; return *this; }
    vec2& operator*=(const float& other) { x *= other; y *= other; return *this; }
    vec2& operator/=(const float& other) { x /= other; y /= other; return *this; }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec2(const glm::vec2& other) : x(other.x), y(other.y) {}
    glm::vec2 toglm(){ return glm::vec2(x, y); }
#endif
};

struct vec4 {
    float x = 0, y = 0, z = 0, w =0;
    vec4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; }
    vec4() {}
    vec4 operator+(const vec4& other) { return vec4( x + other.x, y + other.y, z + other.z, w + other.w); }
    vec4 operator-(const vec4& other) { return vec4( x - other.x, y - other.y, z - other.z, w - other.w); }
    vec4 operator*(const vec4& other) { return vec4( x * other.x, y * other.y, z * other.z, w * other.w); }
    vec4 operator/(const vec4& other) { return vec4( x / other.x, y / other.y, z / other.z, w / other.w); }

    vec4& operator+=(const vec4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
    vec4& operator-=(const vec4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
    vec4& operator*=(const vec4& other) { x *= other.x; y *= other.y; z *= other.z; w *= other.w; return *this; }
    vec4& operator/=(const vec4& other) { x /= other.x; y /= other.y; z /= other.z; w /= other.w; return *this; }

    vec4 operator+(const float& other) { return vec4( x + other, y + other, z + other, w + other); }
    vec4 operator-(const float& other) { return vec4( x - other, y - other, z - other, w - other); }
    vec4 operator*(const float& other) { return vec4( x * other, y * other, z * other, w * other); }
    vec4 operator/(const float& other) { return vec4( x / other, y / other, z / other, w / other); }

    vec4& operator+=(const float& other) { x += other; y += other; z += other; w += other; return *this; }
    vec4& operator-=(const float& other) { x -= other; y -= other; z -= other; w -= other; return *this; }
    vec4& operator*=(const float& other) { x *= other; y *= other; z *= other; w *= other; return *this; }
    vec4& operator/=(const float& other) { x /= other; y /= other; z /= other; w /= other; return *this; }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec4(const glm::vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    glm::vec4 toglm(){ return glm::vec4(x, y, z, w); }
#endif
};
