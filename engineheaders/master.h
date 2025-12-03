#pragma once

#include <glm/glm.hpp> // For error highlighting

#ifdef GLMPresent 
    #include <bit>
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

    float dot(const vec3& other) const { return x * other.x + y * other.y + z * other.z; }
    vec3 cross(const vec3& other) const { return vec3((y * other.z) - (z * other.y), (z * other.x) - (x * other.z), (x * other.y) - (y * other.x)); }

    float length() const { return std::sqrt(x*x + y*y + z*z); }
    vec3 norm() const { float Length = length(); return vec3(x / Length, y / Length, z / Length); }
    
#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec3 (const glm::vec3& other) : x(other.x), y(other.y), z(other.z) {}
    glm::vec3 toglm() const { return glm::vec3(x, y, z); }
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

    float dot(const vec2& other) const { return x * other.x + y * other.y; }
    float cross(const vec2& other) const { return (x * other.y) - (y * other.x); }

    float length() const { return std::sqrt(x*x + y*y); }
    vec2 norm() const { float Length = length(); return vec2(x / Length, y / Length); }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec2 (const glm::vec2& other) : x(other.x), y(other.y) {}
    glm::vec2 toglm() const { return glm::vec2(x, y); }
#endif
};

struct vec4 {
    float x = 0, y = 0, z = 0, w = 0;
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

    float dot(const vec4& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }

    float length() const { return std::sqrt(x*x + y*y + z*z + w*w); }
    vec4 norm() const { float Length = length(); return vec4(x / Length, y / Length, z / Length, 2 / Length); }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec4 (const glm::vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    glm::vec4 toglm() const { return glm::vec4(x, y, z, w); }
#endif
};



struct mat4 {
    mat4(float a = 1, float b = 0, float c = 0, float d = 0,
           float e = 0, float f = 1, float g = 0, float h = 0,
           float i = 0, float j = 0, float k = 1, float l = 0,
           float m = 0, float n = 0, float o = 0, float p = 1) {
        data[0][0] = a; data[1][0] = b; data[2][0] = c; data[3][0] = d;
        data[0][1] = e; data[1][1] = f; data[2][1] = g; data[3][1] = h;
        data[0][2] = i; data[1][2] = j; data[2][2] = k; data[3][2] = l;
        data[0][3] = m; data[1][3] = n; data[2][3] = o; data[3][3] = p;
    }

    float& operator[](int row, int column) {
        return data[column][row];
    }

    mat4 operator*(const mat4& other) {
        mat4 result;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                result[x, y] = multiplySlot(other, x, y);
            }
        }
        return result;
    }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    mat4(const glm::mat4& other) {
        *this = std::bit_cast<mat4>(other);
    } 
    glm::mat4 toglm() {
        return std::bit_cast<glm::mat4>(*this);
    }
#endif

private:
    inline float multiplySlot(const mat4& other, int row, int column) {
        return data[0][row] * other.data[column][0] + data[1][row] * other.data[column][1] + data[2][row] * other.data[column][2] + data[3][row] * other.data[column][3];
    } 

    float data[4][4];
};


namespace Shapes {
    struct AABB {
        vec3 Min = vec3(0, 0, 0);
        vec3 Max = vec3(1, 1, 1); 
    };
    struct Sphere {
        vec3 Position = vec3(0, 0, 0);
        float Radius  = 0;
    };
    struct Line {
        vec3 Start = vec3(0, 0, 0);
        vec3 End   = vec3(1, 1, 1);
    };
}