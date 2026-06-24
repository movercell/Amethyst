#version 460 core
#extension GL_ARB_bindless_texture : require

#include "STDGLLight.incl"
#include "STDGLCamera.incl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormalAndPBR;
in VertexData {
    vec3 Position;
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
    return isOther != 0u && isOther != 3u ? vec4(ret.xyz, 1.0f) : vec4((ret * 1.0f).xyz, 1.0f);
}

void main()
{
    const float near = 1.0f;
    const float far = 32768.0f;
    float linearDepth = (2.0 * near * far) / (far + near - gl_FragCoord.z * (far - near));
    linearDepth /= 100.0f;
    vec4 Albedo = GenerateTemporaryTexture() * vec4(0.3f * linearDepth, 0.3f * linearDepth, 0.7f * linearDepth, 1.0f);

    vec4 WorldPositionNonLinear = Camera.InverseViewProjection * LocalPosition;
    vec3 WorldPosition = WorldPositionNonLinear.xyz / WorldPositionNonLinear.w;
    vec4 LightPos = LightBuffer.Lights[0].ViewProjection * vec4(WorldPosition, 1.0f);
    vec3 projCoords = LightPos.xyz / LightPos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float Shadow = 0.0f;
    
    if (dot(normalize(Normal), normalize(LightBuffer.Lights[0].LightPos - WorldPosition)) >= 0.0f)
        Shadow = texture(sampler2DShadow(LightBuffer.Lights[0].Texture), projCoords);

    Shadow += 0.6f;
    Shadow = min(1.0f, Shadow);

    Albedo *= vec4(Shadow, Shadow, Shadow, 1.0f);
    FragColor = Albedo;
    FragNormalAndPBR = vec4(Normal.xy, 0.0f, 0.0f);
}

    //FragColor = vec4(LightPos.x, LightPos.y, LightRecordedDepth, 1.0f);
    //FragColor = vec4(LightRecordedDepth, LightRecordedDepth, LightRecordedDepth, 1.0f);
