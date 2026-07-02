#version 460 core

#include "STDGLModel.incl"
#include "STDGLCamera.incl"

layout(local_size_x = STDGLMODEL_INSTANCE_PREPROCESS_GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

bool InvPlaneTest(vec3 center, float radius, vec4 plane) {
    float Distance = dot(plane.xyz, center) + plane.w;
        
    if (Distance < -radius) {
        return true;
    }
    return false;
}

void main() {
    bool isActive = true;

    if (isnan(InstanceBuffer.InstanceMatrices[gl_GlobalInvocationID.x][0][0])) isActive = false;


    vec3 position = InstanceBuffer.InstanceMatrices[gl_GlobalInvocationID.x][3].xyz;
    float maxscale;

    {
        float xscale = length(InstanceBuffer.InstanceMatrices[gl_GlobalInvocationID.x][0].xyz);
        float yscale = length(InstanceBuffer.InstanceMatrices[gl_GlobalInvocationID.x][1].xyz);
        float zscale = length(InstanceBuffer.InstanceMatrices[gl_GlobalInvocationID.x][2].xyz);

        maxscale = max(max(xscale, yscale), zscale);
    }

    float radius = ModelInfo.Radius * maxscale;

    for (int i = 0; i < 6; i++)
        if (InvPlaneTest(position, radius, Camera.Frustum[i]))
            isActive = false;


    // Determine the LOD level
    float DistanceFromCamera = distance(Camera.Pos, vec3(InstanceBuffer.InstanceMatrices[gl_GlobalInvocationID.x][3]));
    int LOD = STDGLMODEL_LOD_MAX_COUNT - 1;
    while ((LOD != 0) && (DistanceFromCamera < (ModelInfo.LODDistances[LOD] * maxscale))) LOD--;

    if (isActive) {
        uint ID = atomicAdd(ModelInfo.IndirectBuffers[LOD][0].instanceCount, 1u);
        ModelInfo.InstanceIndices[LOD][ID] = gl_GlobalInvocationID.x;
    }
}