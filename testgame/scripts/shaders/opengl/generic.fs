#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLLight.incl"
#include "STDGLCamera.incl"
#include "OctahedralMapping.incl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 FragNormal;
in VertexData {
    vec4 Position;
    vec4 LocalPosition;
    vec3 Normal;
    vec2 UV;
};

vec3 GenerateTemporaryTexture() {
    uint isOther = 0u;
    float X = UV.x;
    float Y = UV.y;

    while (X > 1.0f) X -= 1.0f;
    while (Y > 1.0f) Y -= 1.0f;
    while (X < 0.0f) X += 1.0f;
    while (Y < 0.0f) Y += 1.0f;

    if (X >= 0.5f)
        isOther = 1u;
    if (Y >= 0.5f)
        isOther += 2u;
    vec3 ret = vec3(1.0f);
    return isOther != 0u && isOther != 3u ? ret : ret * 0.9f;
}

void main()
{
    //const float near = 1.0f;
    //const float far = 32768.0f;
    //float linearDepth = (2.0 * near * far) / (far + near - gl_FragCoord.z * (far - near));
    //linearDepth /= 100.0f;

    float linearDepth = 1.0f;
    vec3 Albedo = GenerateTemporaryTexture() * vec3(0.3f * linearDepth, 0.3f * linearDepth, 0.7f * linearDepth);

    FragColor = vec4(STDGLight_ProcessSpotlight(Albedo, normalize(Normal), 0.05f, 0.7f, vec3(Position.xyz), 0) + (Albedo * vec3(0.3f)), 1.0f);
    FragNormal = Octahedral_Map(normalize(Normal));
}
