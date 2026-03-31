#pragma once

#include "engine/master.h"

namespace Shapes {
    class Frustum;
}

class alignas(alignof(vec4)) Shapes::Frustum {
    vec4 Right;
    vec4 Left;
    vec4 Top;
    vec4 Bottom;
    vec4 Back;
    vec4 Front;

    inline void NormalizePlane(vec4& plane) {
        float magnitude = reinterpret_cast<vec3*>(&plane)->length();

        plane.x /= magnitude;
        plane.y /= magnitude;
        plane.z /= magnitude;
        plane.w /= magnitude;
    };

public:
    Frustum() {};
    Frustum(const mat4& ViewProjection) {
        Right.x = ViewProjection[0, 3] - ViewProjection[0, 0];
        Right.y = ViewProjection[1, 3] - ViewProjection[1, 0];
        Right.z = ViewProjection[2, 3] - ViewProjection[2, 0];
        Right.w = ViewProjection[3, 3] - ViewProjection[3, 0];

        NormalizePlane(Right);

        Left.x = ViewProjection[0, 3] + ViewProjection[0, 0];
        Left.y = ViewProjection[1, 3] + ViewProjection[1, 0];
        Left.z = ViewProjection[2, 3] + ViewProjection[2, 0];
        Left.w = ViewProjection[3, 3] + ViewProjection[3, 0];

        NormalizePlane(Left);

        Top.x = ViewProjection[0, 3] - ViewProjection[0, 1];
        Top.y = ViewProjection[1, 3] - ViewProjection[1, 1];
        Top.z = ViewProjection[2, 3] - ViewProjection[2, 1];
        Top.w = ViewProjection[3, 3] - ViewProjection[3, 1];

        NormalizePlane(Top);

        Bottom.x = ViewProjection[0, 3] + ViewProjection[0, 1];
        Bottom.y = ViewProjection[1, 3] + ViewProjection[1, 1];
        Bottom.z = ViewProjection[2, 3] + ViewProjection[2, 1];
        Bottom.w = ViewProjection[3, 3] + ViewProjection[3, 1];

        NormalizePlane(Bottom);

        Back.x = ViewProjection[0, 3] - ViewProjection[0, 2];
        Back.y = ViewProjection[1, 3] - ViewProjection[1, 2];
        Back.z = ViewProjection[2, 3] - ViewProjection[2, 2];
        Back.w = ViewProjection[3, 3] - ViewProjection[3, 2];

        NormalizePlane(Back);

        Front.x = ViewProjection[0, 3] + ViewProjection[0, 2];
        Front.y = ViewProjection[1, 3] + ViewProjection[1, 2];
        Front.z = ViewProjection[2, 3] + ViewProjection[2, 2];
        Front.w = ViewProjection[3, 3] + ViewProjection[3, 2];

        NormalizePlane(Front);
    }
};

/* ViewProjection[, ]
[ 0]
[ 1]
[ 2]
[ 3]

[ 4]
[ 5]
[ 6]
[ 7]

[ 8]
[ 9]
[10]
[11]

[12]
[13]
[14]
[15] */