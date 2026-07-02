#version 460 core

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};
out _VertexData {
    vec2 UV;
    vec2 LocalPosition;
} toFrag;

const vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0), vec2( 1.0, -1.0), vec2(-1.0,  1.0),
    vec2(-1.0,  1.0), vec2( 1.0, -1.0), vec2( 1.0,  1.0)
);

void main() {
    vec2 pos = positions[gl_VertexID];
    
    toFrag.LocalPosition = pos;
    toFrag.UV = pos * 0.5 + 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}