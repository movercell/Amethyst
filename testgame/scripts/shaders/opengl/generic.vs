#version 460 core

layout (location = 0) in vec3 aPos;


layout (std140, binding = 0) uniform Camera {
    mat4 CameraMatrix;
};

#define STDGLMODEL_INSTANCE_MAX_COUNT 4096
#define STDGLMODEL_LOD_MAX_COUNT 1
#define STDGLMODEL_MESH_MAX_COUNT 8

struct IndirectDrawBuffer {
    uint count;
    uint instanceCount;
    uint firstIndex;
    int baseVertex;
    uint baseInstance;
};

layout (std430, binding = 0) buffer Instances {
    IndirectDrawBuffer IndirectBuffers[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT]; 
    mat4 InstanceMatrices[STDGLMODEL_INSTANCE_MAX_COUNT];
};

void main() {
    gl_Position = CameraMatrix * InstanceMatrices[gl_InstanceID] * vec4(aPos, 1.0f);
}
