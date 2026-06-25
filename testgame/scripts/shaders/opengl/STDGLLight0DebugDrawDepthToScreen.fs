#version 460 core
#extension GL_ARB_bindless_texture : require

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "STDGLLight.incl"
#include "STDGLCamera.incl"

layout (location = 0) out vec4 FragColor;

in VertexData {
    vec2 UV;
    vec2 LocalPosition;
};

void main() {
    float LightRecordedDepth = texture(sampler2D(LightBuffer.Lights[0].Texture), UV).r;

    float z = LightRecordedDepth * 2.0 - 1.0; // Back to NDC 
    LightRecordedDepth = (2.0 * LightBuffer.Lights[0].Near * LightBuffer.Lights[0].Far) / (LightBuffer.Lights[0].Far + LightBuffer.Lights[0].Near - z * (LightBuffer.Lights[0].Far - LightBuffer.Lights[0].Near));
    LightRecordedDepth /= LightBuffer.Lights[0].Far;

    FragColor = vec4(LightRecordedDepth, LightRecordedDepth, LightRecordedDepth, 1.0f);
}