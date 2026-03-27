#version 460 core

#include "STDGLModel.incl"

layout(local_size_x = STDGLMODEL_LOD_MAX_COUNT, local_size_y = STDGLMODEL_MESH_MAX_COUNT, local_size_z = 1) in;

#define BUFFER IndirectBuffers[gl_LocalInvocationID.x][gl_LocalInvocationID.y]

void main() {
    BUFFER.instanceCount = STDGLMODEL_INSTANCE_MAX_COUNT;
    BUFFER.baseInstance = gl_LocalInvocationID.x;
}