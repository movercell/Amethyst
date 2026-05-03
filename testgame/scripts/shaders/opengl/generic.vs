#version 460 core

layout (location = 0) in vec3 aPos;


layout (std140, binding = 0) uniform Camera {
    mat4 CameraView;
    mat4 CameraViewProjection;
    vec4 CameraFrustum[6];
};

#define STDGLMODEL_INSTANCE_MAX_COUNT 4096
#define STDGLMODEL_LOD_MAX_COUNT 4
#define STDGLMODEL_MESH_MAX_COUNT 8

struct IndirectDrawBuffer {
    uint count;
    uint instanceCount;
    uint firstIndex;
    int baseVertex;
    uint baseInstance;
};

layout (std430, binding = 0) buffer ModelInfo {
    IndirectDrawBuffer IndirectBuffers[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_MESH_MAX_COUNT]; 
    float ModelRadius;
    float LODDistances[STDGLMODEL_LOD_MAX_COUNT];
    uint InstanceIndices[STDGLMODEL_LOD_MAX_COUNT][STDGLMODEL_INSTANCE_MAX_COUNT];
};

layout (std430, binding = 1) buffer InstanceBuffer {
    mat4 InstanceMatrices[];
};

void main() {
    gl_Position = CameraViewProjection * InstanceMatrices[InstanceIndices[gl_BaseInstance][gl_InstanceID]] * vec4(aPos, 1.0f);
}
