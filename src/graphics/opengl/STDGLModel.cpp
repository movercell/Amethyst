#include "STDGLCamera.h"
#include "engine/master.h"
#include "STDGLModel.h"

STDGLModel::STDGLModel(std::string path = "error.glb") {
    Geometry::Model model(path);
    Meshes.reserve(model.Meshes.size());
    for (auto mesh : model.Meshes) {
        Meshes.push_back(std::move(STDGLMesh(mesh)));
    }
}

STDGLMesh::STDGLMesh(const Geometry::Mesh& mesh) {
    MeshInfo_t Info;
    glCreateBuffers(3, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glNamedBufferData(VBO, mesh.Vertices.size() * sizeof(Shapes::Vertex), mesh.Vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glNamedBufferData(EBO, mesh.Indeces.size() * sizeof(GLuint), mesh.Indeces.data(), GL_STATIC_DRAW);
    IndexCount = mesh.Indeces.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    Info.Radius = mesh.Radius;

    glNamedBufferData(MeshInfo, sizeof(MeshInfo_t), &Info, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Shapes::Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Shapes::Vertex), (void*)offsetof(Shapes::Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Shapes::Vertex), (void*)offsetof(Shapes::Vertex, TexCoords));
    
}

STDGLModel::~STDGLModel() {
    for (auto mesh : Meshes) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(2, &mesh.VBO);
    }
}

void STDGLModel::Draw() {
    for (auto mesh : Meshes) {
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, mesh.MeshInfo);
        glBindVertexArray(mesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, 0, 4096);
    }
}




void STDGLModelInstance::SetMatrix(mat4 Matrix) {
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), &Matrix);
}

STDGLModelInstance::~STDGLModelInstance() {
    uint8_t temparr[sizeof(mat4)] = { 0xFF, 0x80, 0x00, 0x00 }; // To init data to NaN
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), temparr);

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
    glfwMakeContextCurrent(rendererData);

    glDeleteBuffers(1, &InstanceBuffer);
}