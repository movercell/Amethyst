#version 460 core

#include "STDGLModel.incl"

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;


void main() {
    if (gl_GlobalInvocationID.x == 0)
        for (int LOD = 0; LOD < STDGLMODEL_LOD_MAX_COUNT; LOD++)
            IndirectBuffers[LOD][0].instanceCount = 0;
    memoryBarrierBuffer();

    if (isnan(InstanceMatrices[gl_GlobalInvocationID.x][0][0])) return;

    int LOD = 0; //temp

    uint ID = atomicAdd(IndirectBuffers[LOD][0].instanceCount, 1u);

    InstanceIndeces[LOD][ID] = gl_GlobalInvocationID.x;
}