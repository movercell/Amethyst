#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "Amethyst_SampleTexture.incl"
#include "STDGLCamera.incl"
#include "STDGLLight.incl"
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
    uvec2 AlbedoTexture;
    uvec2 NormalTexture;
    uvec2 MRMapTexture;
} Material;

void main()
{
    mat3 TBN = mat3(
        normalize(VertexData.TBN[0]),
        normalize(VertexData.TBN[1]),
        normalize(VertexData.TBN[2])
    );

    vec3 Albedo = Amethyst_SampleTexture(Material.AlbedoTexture, VertexData.UV).xyz;
    vec2 RawTexNormal = Amethyst_SampleTexture(Material.NormalTexture, VertexData.UV).rg * 2.0 - 1.0;
    vec3 TexNormal = vec3(RawTexNormal, sqrt(1 - RawTexNormal.x * RawTexNormal.x - RawTexNormal.y * RawTexNormal.y));
    vec4 MRMap = Amethyst_SampleTexture(Material.MRMapTexture, VertexData.UV);

    vec3 IncomingLight;

    vec3 Normal = normalize(TBN * TexNormal);

    for (int light = 0; light < STDGLLIGHT_MAX_COUNT; light++) {
        STDGLLightData Light = LightBuffer.Lights[light];

        if (Light.Type == LIGHT_TYPE_SPOT)
            IncomingLight += STDGLight_ProcessSpotlight(Albedo, TBN[2], Normal, MRMap.r, MRMap.g, vec3(VertexData.Position.xyz), Light);
    }
    vec3 AmbientLight = vec3(0.03) * Albedo;

    vec3 Color = AmbientLight + IncomingLight;
	
    Color = Color / (Color + vec3(1.0));
    Color = pow(Color, vec3(1.0/2.2));  

    FragColor = vec4(Color, 1.0f);
    FragNormal = Octahedral_Map(Normal);
}
