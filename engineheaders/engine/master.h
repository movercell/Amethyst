#pragma once
#include <cmath>
#include <array>
#include <cstdint>
//#include <glm/glm.hpp> // For error highlighting

#ifdef GLMPresent 
    #include <bit>
#endif

#ifdef _WIN32
    #ifdef AMETHYSTENGINESRC
        #define ENGINEEXPORT __declspec(dllexport)
        #define GAMEEXPORT __declspec(dllimport)
    #else
        #define ENGINEEXPORT __declspec(dllimport)
        #define GAMEEXPORT __declspec(dllexport)
    #endif 
#else
    #define ENGINEEXPORT __attribute__ ((visibility ("default")))
    #define GAMEEXPORT __attribute__ ((visibility ("default")))
#endif


namespace Engine {
    
}

struct alignas(sizeof(float) * 4) vec3 {
    float x = 0, y = 0, z = 0;
    vec3(const float& X, const float& Y, const float& Z) { x = X; y = Y; z = Z; }
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
    vec3 norm() const { float Length = length(); if (Length == 0.0f) return vec3(); return vec3(x / Length, y / Length, z / Length); }
    
#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec3 (const glm::vec3& other) : x(other.x), y(other.y), z(other.z) {}
    glm::vec3 toglm() const { return glm::vec3(x, y, z); }
#endif
};

struct alignas(sizeof(float) * 2) vec2 {
    float x = 0, y = 0;
    vec2(const float& X, const float& Y) { x = X; y = Y; }
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
    vec2 norm() const { float Length = length(); if (Length == 0.0f) return vec2();  return vec2(x / Length, y / Length); }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec2 (const glm::vec2& other) : x(other.x), y(other.y) {}
    glm::vec2 toglm() const { return glm::vec2(x, y); }
#endif
};

struct alignas(sizeof(float) * 4) vec4 {
    float x = 0, y = 0, z = 0, w = 0;
    vec4(const float& X, const float& Y, const float& Z, const float& W) { x = X; y = Y; z = Z; w = W; }
    vec4() {}
    vec4(vec3 other, float W = 1.0f) { x = other.x; y = other.y; z = other.z; w = W; }
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
    vec4 norm() const { float Length = length(); if (Length == 0.0f) return vec4();  return vec4(x / Length, y / Length, z / Length, 2 / Length); }

#if defined(AMETHYSTENGINESRC) && defined(GLMPresent)
    vec4 (const glm::vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    glm::vec4 toglm() const { return glm::vec4(x, y, z, w); }
#endif
};



struct alignas(sizeof(float) * 4) mat4 {
    mat4(const float& a = 1, const float& b = 0, const float& c = 0, const float&  d = 0,
         const float& e = 0, const float& f = 1, const float& g = 0, const float&  h = 0,
         const float& i = 0, const float& j = 0, const float& k = 1, const float&  l = 0,
         const float& m = 0, const float& n = 0, const float& o = 0, const float&  p = 1) {
        data[0][0] = a; data[1][0] = b; data[2][0] = c; data[3][0] = d;
        data[0][1] = e; data[1][1] = f; data[2][1] = g; data[3][1] = h;
        data[0][2] = i; data[1][2] = j; data[2][2] = k; data[3][2] = l;
        data[0][3] = m; data[1][3] = n; data[2][3] = o; data[3][3] = p;
    }

    float& operator[](int column, int row) {
        return data[column][row];
    }

    mat4 operator*(const mat4& other) const {
        mat4 result;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                result[x, y] = multiplySlot(other, x, y);
            }
        }
        return result;
    }

    bool operator==(const mat4& other) const {
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                if (data[col][row] != other.data[col][row]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const mat4& other) const {
        return !(*this == other);
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
    inline float multiplySlot(const mat4& other, int column, int row) const {
        return data[0][row] * other.data[column][0] + data[1][row] * other.data[column][1] + data[2][row] * other.data[column][2] + data[3][row] * other.data[column][3];
    } 

    float data[4][4];
};

struct alignas(sizeof(float) * 4) quat {
    float w, x, y, z;

    quat(float W, float X, float Y, float Z) { w = W; x = X; y = Y; z = Z; Norm(); }
    quat(vec3 angles) {
        const float anglestoradians = 0.017453293;

        float pitchradian = (angles.x * anglestoradians) / 2;
        float yawradian   = (angles.y * anglestoradians) / 2;
        float rollradian  = (angles.z * anglestoradians) / 2;

        quat yaw = quat(cos(yawradian), 0, 0, -sin(yawradian)); // Seemingly I screwed something up and now I have to invert the thing here
        quat pitch = quat(cos(pitchradian), 0, sin(pitchradian), 0);
        quat roll = quat(cos(rollradian), sin(rollradian), 0, 0);

        *this = yaw * (pitch * roll);
        Norm();
    }

    void Norm() {
        float squaremagnitude = w*w + x*x + y*y + z*z;
        float scalefactor;

        if (abs(1 - squaremagnitude) < 2.107342e-08) {
            scalefactor = 2 / (1 + squaremagnitude);
        } else {
            scalefactor = 1 / sqrt(squaremagnitude);
        }

        w *= scalefactor;
        x *= scalefactor;
        y *= scalefactor;
        z *= scalefactor;
    }

    //https://www.songho.ca/opengl/gl_quaternion.html
    mat4 MakeRotationMatrix() {
        return mat4(
           1 - 2 * pow(y, 2) - 2 * pow(z, 2),   2 * x * y - 2 * w * z,              2 * x * z + 2 * w * y, 0,
           2 * x * y + 2 * w * z,               1 - 2 * pow(x, 2) - 2 * pow(z, 2),  2 * y * z - 2 * w * x, 0,
           2 * x * z - 2 * w * y,               2 * y * z + 2 * w * x,              1 - 2 * pow(x, 2) - 2 * pow(y, 2), 0,
           0, 0, 0, 1
        );
    }

    mat4 MakeInvRotationMatrix() {
        return mat4(
            1 - 2 * pow(y, 2) - 2 * pow(z, 2),  2 * x * y + 2 * w * z,              2 * x * z - 2 * w * y, 0,
            2 * x * y - 2 * w * z,              1 - 2 * pow(x, 2) - 2 * pow(z, 2),  2 * y * z + 2 * w * x, 0,
            2 * x * z + 2 * w * y,              2 * y * z - 2 * w * x,              1 - 2 * pow(x, 2) - 2 * pow(y, 2), 0,
            0, 0, 0, 1
        );
    }


    quat operator*(const quat& other) {
        return quat(
            w * other.w + x * other.x + y * other.y + z * other.z,
            w * other.x - x * other.w - y * other.z + z * other.y,
            w * other.y + x * other.z - y * other.w - z * other.x,
            w * other.z - x * other.y + y * other.x - z * other.w
        );
    }
    quat& operator*=(const quat& other) {
        
        w = w * other.w + x * other.x + y * other.y + z * other.z;
        x = w * other.x - x * other.w - y * other.z + z * other.y;
        y = w * other.y + x * other.z - y * other.w - z * other.x;
        z = w * other.z - x * other.y + y * other.x - z * other.w;
        
        return *this;
    }
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
    struct Vertex {
        vec3 Position;
        vec3 Normal;
        vec2 TexCoords;
    };
    struct Triangle {
        std::array<Vertex, 3> Vertices;
    };
}
