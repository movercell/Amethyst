#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLCamera.incl"
#include "STDGLLight.incl"
#include "OctahedralMapping.incl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 FragNormal;
in VertexData {
    vec4 Position;
    vec4 LocalPosition;
    vec3 Normal;
    vec2 UV;
};
layout (std140, binding = 1) uniform _Material {
    float red;
    float green;
    float blue;
} Material;

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
    vec3 Texture = GenerateTemporaryTexture();
    vec3 Albedo = Texture * vec3(Material.red, Material.green, Material.blue);

    vec3 IncomingLight;

    float TexNormX = 0.0f; //  * 2.0 - 1.0
    float TexNormY = 0.0f;
    vec3 TexNormal = vec3(TexNormX, TexNormY, sqrt(1 - TexNormX * TexNormX - TexNormY * TexNormY));

    for (int light = 0; light < STDGLLIGHT_MAX_COUNT; light++) {
        STDGLLightData Light = LightBuffer.Lights[light];

        if (Light.Type == LIGHT_TYPE_SPOT)
            IncomingLight += STDGLight_ProcessSpotlight(Albedo, normalize(Normal), TexNormal, Texture.z * 1.1f, (1.0f - Texture.z * 0.3f), vec3(Position.xyz), Light);
    }
    vec3 AmbientLight = vec3(0.03) * Albedo;

    vec3 Color = AmbientLight + IncomingLight;
	
    Color = Color / (Color + vec3(1.0));
    Color = pow(Color, vec3(1.0/2.2));  

    FragColor = vec4(Color, 1.0f);
    FragNormal = Octahedral_Map(normalize(Normal)); // TODO: This should be combined with the texture normal.
}
