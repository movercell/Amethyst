#version 460 core
#extension GL_ARB_bindless_texture : require

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLLight.incl"
#include "STDGLCamera.incl"
#include "OctahedralMapping.incl"

layout (binding = 0) uniform sampler2D ColorAttachment;
layout (binding = 1) uniform sampler2D NormalAndPBRAttachment;
layout (binding = 2) uniform sampler2D DepthAttachment;
layout (location = 0) out vec4 FragColor;

in _VertexData {
    vec2 UV;
    vec2 LocalPosition;
} VertexData;

void main() {
    float Depth = texelFetch(DepthAttachment, ivec2(gl_FragCoord.xy), 0).r * 2.0f - 1.0f;
    vec4 LocalPosition = vec4(VertexData.LocalPosition.xy, Depth, 1.0f);
    vec4 WorldPositionNonLinear = Camera.InverseViewProjection * LocalPosition;
    vec3 WorldPosition = WorldPositionNonLinear.xyz / WorldPositionNonLinear.w;
    vec4 LightPos = LightBuffer.Lights[0].ViewProjection * vec4(WorldPosition, 1.0f);
    vec3 projCoords = LightPos.xyz / LightPos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float Shadow = 0.0f;

    float NormalDotLightDir = dot(Octahedral_Unmap(vec2(texelFetch(NormalAndPBRAttachment, ivec2(gl_FragCoord.xy), 0).rg)), normalize(LightBuffer.Lights[0].LightPos - WorldPosition));
    if (NormalDotLightDir >= 0.0f && (projCoords.z > 0.0f && projCoords.z < 1.0f)) {
        float bias = max(0.005 * (1.0f - NormalDotLightDir), 0.001);
        Shadow = texture(sampler2DShadow(LightBuffer.Lights[0].Texture), projCoords - bias);
    }

    Shadow += 0.6f;
    Shadow = min(1.0f, Shadow);

    FragColor = texelFetch(ColorAttachment, ivec2(gl_FragCoord.xy), 0) * vec4(Shadow, Shadow, Shadow, 1.0f);
}