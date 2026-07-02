#version 460 core

#include "STDGLModel.incl"

layout(local_size_x = STDGLMODEL_LOD_MAX_COUNT, local_size_y = STDGLMODEL_MESH_MAX_COUNT, local_size_z = 1) in;

void main() {
    ModelInfo.IndirectBuffers[gl_LocalInvocationID.x][gl_LocalInvocationID.y].instanceCount = ModelInfo.IndirectBuffers[gl_LocalInvocationID.x][0].instanceCount;
}