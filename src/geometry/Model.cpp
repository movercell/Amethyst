#include "engine/geometry/Model.h"

#include <assimp/scene.h>
#include "engine/filesystem/Filesystem.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <iostream>

Geometry::Model::Model(std::string path) {
    auto modelfile = Filesystem::GetFileAsStream("models/" + path, std::ios::in | std::ios_base::binary);
    if (!modelfile) {
        modelfile = Filesystem::GetFileAsStream("models/error.glb", std::ios::in | std::ios_base::binary);
    }

    const aiScene* scene;
    {
        auto buffer = std::vector<char>{std::istreambuf_iterator<char>{modelfile}, {}};
        scene = aiImportFileFromMemory(buffer.data(), buffer.size(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes, path.c_str());
    };
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
        aiReleaseImport(scene);
        modelfile = Filesystem::GetFileAsStream("models/error.glb", std::ios::in | std::ios_base::binary);

        auto buffer = std::vector<char>{std::istreambuf_iterator<char>{modelfile}, {}};
        scene = aiImportFileFromMemory(buffer.data(), buffer.size(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes, path.c_str());

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
            Engine::Error(std::string("Error loading the error model: ") + aiGetErrorString());
        }
    }
    
    Meshes.reserve(scene->mNumMeshes);
    for (int meshindex = 0; meshindex < scene->mNumMeshes; meshindex++) {
        Mesh mesh(reinterpret_cast<void*>(scene->mMeshes[meshindex]));
        Meshes.push_back(std::move(mesh));
    }
    aiReleaseImport(scene);
}

Geometry::Mesh::Mesh(void* Meshdata) {
    auto paimesh = reinterpret_cast<aiMesh*>(Meshdata);

    Vertices.reserve(paimesh->mNumVertices);
    for (int vertexindex = 0; vertexindex < paimesh->mNumVertices; vertexindex++) {
        Shapes::Vertex vertex;
        const auto& aipos = paimesh->mVertices[vertexindex];
        vertex.Position = vec3(aipos.x, aipos.y, aipos.z);
        const auto& ainorm = paimesh->mNormals[vertexindex];
        vertex.Normal = vec3(ainorm.x, ainorm.y, ainorm.z);
        vertex.TexCoords = *(reinterpret_cast<vec2*>(&(paimesh->mTextureCoords[0][vertexindex])));
        Radius = std::max(Radius, vertex.Position.length());
        Vertices.push_back(vertex);
    }
    Indices.reserve(paimesh->mNumFaces * 3);
    for (int faceindex = 0; faceindex < paimesh->mNumFaces; faceindex++) {
        for (int indexindex = 0; indexindex < 3; indexindex++) {
            Indices.push_back(paimesh->mFaces[faceindex].mIndices[indexindex]);
        }
    }
}