#version 460 core

layout (location = 0) in vec3 aPos;


layout (std140, binding = 0) uniform Camera {
    mat4 CameraMatrix;
};

layout (std140, binding = 0) buffer Instances {
    mat4 InstanceMatrices[4096];
};

void main() {
    gl_Position = CameraMatrix * InstanceMatrices[gl_InstanceID] * vec4(aPos, 1.0f);
}
