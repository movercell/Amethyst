#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLModel.incl"
#include "STDGLCamera.incl"

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec4 TangentSpace;
out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};
out _VertexData {
    vec4 Position;
    vec4 LocalPosition;
    mat3 TBN;
    vec2 UV;
} toFrag;


void main() {
    mat4 InstanceMatrix = InstanceBuffer.InstanceMatrices[InstanceBuffer.InstanceIndices[gl_BaseInstance][gl_InstanceID]];
    toFrag.UV = UV;
    toFrag.Position = InstanceMatrix * vec4(Position, 1.0f);
    toFrag.LocalPosition = Camera.ViewProjection * toFrag.Position;

    vec3 Tangent = TangentSpace.xyz;
    vec3 Bitangent = cross(Normal, Tangent) * TangentSpace.w;

    mat3 InstanceMatrixForTexture = transpose(inverse(mat3(InstanceMatrix)));
    toFrag.TBN = mat3(
        normalize(InstanceMatrixForTexture * Tangent),
        normalize(InstanceMatrixForTexture * Bitangent),
        normalize(InstanceMatrixForTexture * Normal)
    );

    gl_Position = toFrag.LocalPosition;
}
