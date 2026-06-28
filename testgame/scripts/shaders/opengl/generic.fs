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

vec4 GenerateTemporaryTexture() {
    uint isOther = 0u;
    if (UV.x >= 0.5f)
        isOther = 1u;
    if (UV.y >= 0.5f)
        isOther += 2u;
    vec3 ret = vec3(1.0f);
    return isOther != 0u && isOther != 3u ? vec4(ret.xyz, 1.0f) : vec4((ret * 0.9f).xyz, 1.0f);
}

void main()
{
    //const float near = 1.0f;
    //const float far = 32768.0f;
    //float linearDepth = (2.0 * near * far) / (far + near - gl_FragCoord.z * (far - near));
    //linearDepth /= 100.0f;

    float linearDepth = 1.0f;
    vec4 Albedo = GenerateTemporaryTexture() * vec4(0.3f * linearDepth, 0.3f * linearDepth, 0.7f * linearDepth, 1.0f);

    vec4 LightPos = LightBuffer.Lights[0].ViewProjection * Position;
    vec3 projCoords = LightPos.xyz / LightPos.w;
    projCoords = projCoords * 0.5 + 0.5;

    float NormalDotLightDir = max(dot(normalize(Normal), normalize(LightBuffer.Lights[0].LightPos - vec3(Position.xyz))), 0.0f);
    
    float Shadow = SampleSpotLightForShadows(0, projCoords, NormalDotLightDir);

    Shadow += 0.3f;
    Shadow = min(1.0f, Shadow);

    FragColor = Albedo * vec4(Shadow, Shadow, Shadow, 1.0f);
    FragNormal = Octahedral_Map(normalize(Normal));
}
