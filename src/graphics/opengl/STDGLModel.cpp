#include "STDGLCamera.h"
#include "master.h"
#include "STDGLModel.h"
#include "Filesystem.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

STDGLModel::STDGLModel(std::string name) {
    auto modelfile = Filesystem::GetFile("models/" + name, std::ios::in);
    if (!modelfile) {
        modelfile = Filesystem::GetFile("models/error.glb", std::ios::in);
    }

    Assimp::Importer importer;
    const aiScene* scene;
    std::vector<char> buffer;
    {
        buffer = std::vector<char>((std::istreambuf_iterator<char>(modelfile)), (std::istreambuf_iterator<char>()));
        scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes, name.c_str());
    };
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    
    Meshes.reserve(scene->mNumMeshes);
    for (int meshindex = 0; meshindex < scene->mNumMeshes; meshindex++) {
        STDGLMesh mesh(scene->mMeshes[meshindex]);
        Meshes.push_back(std::move(mesh));
    }
}

STDGLMesh::STDGLMesh(aiMesh* paimesh) {
    MeshInfo_t Info;
    glCreateBuffers(3, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    {
        std::vector<Shapes::Vertex> vertices;
        vertices.reserve(paimesh->mNumVertices);
        for (int vertexindex = 0; vertexindex < paimesh->mNumVertices; vertexindex++) {
            Shapes::Vertex vertex;
            vertex.Position = std::bit_cast<vec3>(paimesh->mVertices[vertexindex]);
            vertex.Normal = std::bit_cast<vec3>(paimesh->mNormals[vertexindex]);
            vertex.TexCoords = *(reinterpret_cast<vec2*>(&(paimesh->mTextureCoords[0][vertexindex])));
            Info.Radius = std::max(Info.Radius, vertex.Position.length());
            vertices.push_back(vertex);
        }
        glNamedBufferData(VBO, vertices.size() * sizeof(Shapes::Vertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }
    {
        std::vector<GLuint> indeces;
        indeces.reserve(paimesh->mNumFaces * 3);
        for (int faceindex = 0; faceindex < paimesh->mNumFaces; faceindex++) {
            for (int indexindex = 0; indexindex < 3; indexindex++) {
                indeces.push_back(paimesh->mFaces[faceindex].mIndices[indexindex]);
            }
        }
        glNamedBufferData(EBO, indeces.size() * sizeof(GLuint), indeces.data(), GL_STATIC_DRAW);
        IndexCount = indeces.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }

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