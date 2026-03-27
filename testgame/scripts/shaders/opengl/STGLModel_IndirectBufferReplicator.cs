#version 460 core

#include "STDGLModel.incl"

layout(local_size_x = STDGLMODEL_LOD_MAX_COUNT, local_size_y = STDGLMODEL_MESH_MAX_COUNT, local_size_z = 1) in;

#define DEST IndirectBuffers[gl_LocalInvocationID.x][gl_LocalInvocationID.y]
#define SRC  IndirectBuffersInfo[gl_LocalInvocationID.x][gl_LocalInvocationID.y]

void main() {
    DEST.count = SRC.count;
    DEST.instanceCount = STDGLMODEL_INSTANCE_MAX_COUNT;
    DEST.firstIndex = SRC.firstIndex;
    DEST.baseVertex = SRC.baseVertex;
    DEST.baseInstance = 0;
}