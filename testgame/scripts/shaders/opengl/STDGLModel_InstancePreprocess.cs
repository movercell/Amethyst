#version 460 core

#define STDGLMODEL_INSTANCE_MAX_COUNT 4096
#define STDGLMODEL_LOD_MAX_COUNT 1
#define STDGLMODEL_MESH_MAX_COUNT 8

layout(local_size_x = STDGLMODEL_LOD_MAX_COUNT, local_size_y = STDGLMODEL_MESH_MAX_COUNT, local_size_z = 1) in;

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
}