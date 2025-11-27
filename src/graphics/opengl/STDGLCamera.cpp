#include "STDGLCamera.h"

void STDGLCamera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.z = sin(glm::radians(Pitch));
    front.y = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front.toglm(), WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right.toglm(), Front.toglm()));
}

void STDGLCamera::Bind(GLuint CameraMatrixBuffer) {
    glm::mat4 view = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
    glm::mat4 projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, 0.1f, 100.0f);
    glm::mat4 viewprojection = projection * view;
    glNamedBufferData(CameraMatrixBuffer, sizeof(glm::mat4), &viewprojection, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, CameraMatrixBuffer);
}

void STDGLCamera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw   += xoffset;
    Pitch -= yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}