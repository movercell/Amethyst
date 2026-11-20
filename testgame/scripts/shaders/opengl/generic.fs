#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLCamera.incl"
#include "STDGLLight.incl"
#include "ApplyTBNToTexture.incl"
#include "OctahedralMapping.incl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 FragNormal;
in _VertexData {
    vec4 Position;
    vec4 LocalPosition;
    mat3 TBN;
    vec2 UV;
} VertexData;
layout (std140, binding = 1) uniform _Material {
    float red;
    float green;
    float blue;
    float gridexponent;
} Material;

vec3 GenerateTemporaryTexture() {
    uint isOther = 0u;
    float X = VertexData.UV.x;
    float Y = VertexData.UV.y;

    while (X > 1.0f) X -= 1.0f;
    while (Y > 1.0f) Y -= 1.0f;
    while (X < 0.0f) X += 1.0f;
    while (Y < 0.0f) Y += 1.0f;

    if (X >= 0.5f)
        isOther = 1u;
    if (Y >= 0.5f)
        isOther += 2u;
    vec3 ret = vec3(1.0f);
    return isOther != 0u && isOther != 3u ? ret : ret * Material.gridexponent;
}

void main()
{
    vec3 Texture = GenerateTemporaryTexture();
    vec3 Albedo = Texture * vec3(Material.red, Material.green, Material.blue);

    vec3 IncomingLight;

    float TexNormX = 0.5f;
    float TexNormY = 0.5f;
    vec3 TexNormal = vec3(TexNormX, TexNormY, 1.0f);//sqrt(1 - TexNormX * TexNormX - TexNormY * TexNormY));
    vec3 Normal = ApplyTBNToTexture(VertexData.TBN, TexNormal);

    for (int light = 0; light < STDGLLIGHT_MAX_COUNT; light++) {
        STDGLLightData Light = LightBuffer.Lights[light];

        if (Light.Type == LIGHT_TYPE_SPOT)
            IncomingLight += STDGLight_ProcessSpotlight(Albedo, VertexData.TBN[2], Normal, Texture.z * 1.1f, (1.0f - Texture.z * 0.3f), vec3(VertexData.Position.xyz), Light);    }
    vec3 AmbientLight = vec3(0.03) * Albedo;

    vec3 Color = AmbientLight + IncomingLight;
	
    Color = Color / (Color + vec3(1.0));
    Color = pow(Color, vec3(1.0/2.2));  

    FragColor = vec4(Color, 1.0f);
    FragNormal = Octahedral_Map(Normal);
}
