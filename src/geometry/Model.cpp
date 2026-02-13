#include "engine/geometry/Model.h"

#include <assimp/scene.h>
#include "engine/Filesystem.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

Geometry::Model::Model(std::string path) {
    auto modelfile = Filesystem::GetFile("models/" + path, std::ios::in);
    if (!modelfile) {
        modelfile = Filesystem::GetFile("models/error.glb", std::ios::in);
    }

    Assimp::Importer importer;
    const aiScene* scene;
    {
        auto buffer = std::vector<char>((std::istreambuf_iterator<char>(modelfile)), (std::istreambuf_iterator<char>()));
        scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes, path.c_str());
    };
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
        modelfile = Filesystem::GetFile("models/error.glb", std::ios::in);

        auto buffer = std::vector<char>((std::istreambuf_iterator<char>(modelfile)), (std::istreambuf_iterator<char>()));
        scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes, path.c_str());

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
            std::cout << "Error loading the error model: " << importer.GetErrorString() << std::endl;
            exit(-1);
        }
    }
    
    Meshes.reserve(scene->mNumMeshes);
    for (int meshindex = 0; meshindex < scene->mNumMeshes; meshindex++) {
        Mesh mesh(reinterpret_cast<void*>(scene->mMeshes[meshindex]));
        Meshes.push_back(std::move(mesh));
    }
}

Geometry::Mesh::Mesh(void* Meshdata) {
    auto paimesh = reinterpret_cast<aiMesh*>(Meshdata);

    Vertices.reserve(paimesh->mNumVertices);
    for (int vertexindex = 0; vertexindex < paimesh->mNumVertices; vertexindex++) {
        Shapes::Vertex vertex;
        vertex.Position = std::bit_cast<vec3>(paimesh->mVertices[vertexindex]);
        vertex.Normal = std::bit_cast<vec3>(paimesh->mNormals[vertexindex]);
        vertex.TexCoords = *(reinterpret_cast<vec2*>(&(paimesh->mTextureCoords[0][vertexindex])));
        Radius = std::max(Radius, vertex.Position.length());
        Vertices.push_back(vertex);
    }
    Indeces.reserve(paimesh->mNumFaces * 3);
    for (int faceindex = 0; faceindex < paimesh->mNumFaces; faceindex++) {
        for (int indexindex = 0; indexindex < 3; indexindex++) {
            Indeces.push_back(paimesh->mFaces[faceindex].mIndices[indexindex]);
        }
    }
}