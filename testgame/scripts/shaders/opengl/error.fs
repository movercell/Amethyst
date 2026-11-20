#version 460 core

#ifdef GLSLANGVALIDATOR
#extension GL_GOOGLE_include_directive : require
#endif

#include "OctahedralMapping.incl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 FragNormal;
in _VertexData {
    vec4 Position;
    vec4 LocalPosition;
    mat3 TBN;
    vec2 UV;
} VertexData;

void main()
{
    FragColor = vec4(0.6f, 0.4f, 0.8f, 1.0f);
    FragNormal = Octahedral_Map(VertexData.TBN[2]);
}
