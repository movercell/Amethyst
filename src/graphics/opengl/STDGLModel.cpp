#include "STDGLCamera.h"
#include "master.h"
#include "STDGLModel.h"

void STDGLModelInstance::SetMatrix(mat4 Matrix) {
    auto temp = glfwGetCurrentContext();
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), &Matrix);

    glfwMakeContextCurrent(temp);
}

STDGLModelInstance::~STDGLModelInstance() {
    auto temp = glfwGetCurrentContext();
    uint8_t temparr[sizeof(mat4)] = { 0xFF, 0x80, 0x00, 0x00 }; // To init data to NaN
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), temparr);

    glfwMakeContextCurrent(temp);

    parent->count--;
    parent->FreedIndeces.push(index);
}

std::shared_ptr<ModelInstance> STDGLModelInstanceArray::MakeModelInstance() {
    uint16_t index;
    if (FreedIndeces.empty()) {
        index = NextIndex;
        NextIndex++;
    } else {
        index = FreedIndeces.front();
        FreedIndeces.pop();
    }
    return std::make_shared<STDGLModelInstance>(index, this);
}

STDGLModelInstanceArray::~STDGLModelInstanceArray() {
    auto temp = glfwGetCurrentContext();
    glfwMakeContextCurrent(rendererData);

    glDeleteBuffers(1, &InstanceBuffer);

    glfwMakeContextCurrent(temp);
}