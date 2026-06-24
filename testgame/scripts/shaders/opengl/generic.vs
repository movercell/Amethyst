#version 460 core

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
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} toFrag;


void main() {
    toFrag.Position = Position;
    toFrag.Normal = Normal;
    toFrag.UV = UV;
    gl_Position = Camera.ViewProjection * InstanceBuffer.InstanceMatrices[ModelInfo.InstanceIndices[gl_BaseInstance][gl_InstanceID]] * vec4(Position, 1.0f);
}
