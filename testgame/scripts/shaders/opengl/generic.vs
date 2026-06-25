#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLModel.incl"
#include "STDGLCamera.incl"

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};
out VertexData {
    vec4 Position;
    vec4 LocalPosition;
    vec3 Normal;
    vec2 UV;
} toFrag;


void main() {
    mat4 InstanceMarix = InstanceBuffer.InstanceMatrices[ModelInfo.InstanceIndices[gl_BaseInstance][gl_InstanceID]];
    toFrag.Normal = transpose(inverse(mat3(InstanceMarix))) * Normal;
    toFrag.UV = UV;
    toFrag.Position = InstanceMarix * vec4(Position, 1.0f);
    toFrag.LocalPosition = Camera.ViewProjection * toFrag.Position;
    gl_Position = toFrag.LocalPosition;
}
