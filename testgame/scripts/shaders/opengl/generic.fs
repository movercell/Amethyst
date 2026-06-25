#version 460 core
#extension GL_ARB_bindless_texture : require

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLLight.incl"
#include "STDGLCamera.incl"
#include "OctahedralMapping.incl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormalAndPBR;
in VertexData {
    vec4 Position;
    vec4 LocalPosition;
    vec3 Normal;
    vec2 UV;
};

vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87922466),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409550, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
);

float LinearizeDepth(float depth, float near, float far) 
{
    float ndc = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
    return linearDepth / far;
}

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
    float Shadow = 0.0f;

    float NormalDotLightDir = max(dot(normalize(Normal), normalize(LightBuffer.Lights[0].LightPos - vec3(Position.xyz))), 0.0f);
    
    if (projCoords.z > 0.0f && projCoords.z < 1.0f) {
        for (int i = 0; i < 16; i++) {
            float bias = max(0.005 * (1.0f - NormalDotLightDir), 0.002);
            Shadow += texture(sampler2DShadow(LightBuffer.Lights[0].Texture), projCoords - bias + vec3(poissonDisk[i] * 0.004f * (1.0f - LinearizeDepth(projCoords.z, LightBuffer.Lights[0].Near, LightBuffer.Lights[0].Far) * 0.7f), 0.0f)) * NormalDotLightDir;
        }
        Shadow /= 16.0f;
    }
    

    Shadow += 0.3f;
    Shadow = min(1.0f, Shadow);

    FragColor = Albedo * vec4(Shadow, Shadow, Shadow, 1.0f);
    FragNormalAndPBR = vec4(Octahedral_Map(normalize(Normal)), 0.0f, 0.0f);
}
