#include "engine/Binaryfile.h"
#include "engine/geometry/ModelFile.h"
#include <fstream>
#include <assimp/scene.h>
#include "engine/Filesystem.h"
#include "engine/ADF.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <iostream>

using GraphicsVertex = Geometry::Modelfile::GraphicsVertex;
using MeshLump_t = Geometry::Modelfile::MeshLump;
using LODLump_t = Geometry::Modelfile::LODLump;
using FlagDefs = Geometry::Modelfile::FlagDefs;

struct ADFSerialize ImportConfigurationLOD {
    std::string Model;
    float Distance;
};

struct ADFSerialize ImportConfigurationFile {
    std::vector<ImportConfigurationLOD> LODs;
    std::map<std::string, std::string> MaterialReplacements;
    std::optional<std::string> CollisionModel; // TODO:
};

// Parameters for tool.
std::filesystem::path inpath = "";
std::filesystem::path outpath = "";
ImportConfigurationFile configfile;

// Storage for all the model data.
std::vector<GraphicsVertex> VertexLump;
std::vector<uint32_t> IndexLump;
std::vector<MeshLump_t> MeshLump;
std::vector<LODLump_t> LODLump;
std::vector<Engine::BinaryFileStringLump> StringLump;
std::vector<vec3> CollisionVertexLump;
std::vector<uint32_t> CollisionIndexLump;
uint64_t Flags = 0;
float Radius = 0.0f;

// Assimp C API ***ONLY***, because libassimp.6.dll is always compiled with MinGW!

inline constexpr unsigned int AI_POSTPROCESS_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace;

const aiScene* MakeAssimpScene(std::string path) {
    auto modelfile = Engine::GetFileAsStream(path, std::ios_base::binary);
    if (!modelfile) {
        Engine::Error("Failed opening file!(" + path + ")");
    }

    std::vector<char> buffer;
    // Reserve the needed space.
    modelfile.seekg(0, std::ios::end);
    buffer.resize(modelfile.tellg());
    modelfile.seekg(0, std::ios::beg);
    
    buffer.assign(std::istreambuf_iterator<char>{modelfile}, {});

    const aiScene* scene;
    scene = aiImportFileFromMemory(buffer.data(), buffer.size(), AI_POSTPROCESS_FLAGS, path.c_str());
    
    // Check for errors.
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Engine::Error("Bad model file!");
    }

    return scene;
}

class ImportedModel;

class ImportedMesh {
protected:
    friend class ImportedModel;

    ImportedMesh(void* Meshdata, const void* Scenedata) {
        auto paimesh = reinterpret_cast<aiMesh*>(Meshdata);
        auto paiscene = reinterpret_cast<const aiScene*>(Scenedata);

        Vertices.reserve(paimesh->mNumVertices);
        for (int vertexindex = 0; vertexindex < paimesh->mNumVertices; vertexindex++) {
            Geometry::Modelfile::GraphicsVertex vertex;
            const auto& aipos = paimesh->mVertices[vertexindex];
            vertex.Position = vec3(aipos.x, aipos.y, aipos.z);
            const auto& ainorm = paimesh->mNormals[vertexindex];
            vertex.Normal = vec3(ainorm.x, ainorm.y, ainorm.z);
            vertex.TexCoords = *(reinterpret_cast<vec2*>(&(paimesh->mTextureCoords[0][vertexindex])));

            const auto& aitang = paimesh->mTangents[vertexindex];
            const auto& aibitang = paimesh->mBitangents[vertexindex];
            vec3 Tangent = vec3(aitang.x, aitang.y, aitang.z);
            vec3 Bitangent = vec3(aibitang.x, aibitang.y, aibitang.z);
            float IsBitangentInverted = vertex.Normal.cross(Tangent).dot(Bitangent) > 0.0f ? 1.0f : -1.0f;
            vertex.Tangent = vec4(Tangent, IsBitangentInverted); 

            Radius = std::max(Radius, vertex.Position.length());

            Vertices.push_back(vertex);
        }
        Indices.reserve(paimesh->mNumFaces * 3);
        for (int faceindex = 0; faceindex < paimesh->mNumFaces; faceindex++) {
            for (int indexindex = 0; indexindex < 3; indexindex++) {
                Indices.push_back(paimesh->mFaces[faceindex].mIndices[indexindex]);
            }
        }
        aiString aiMaterialName;
        aiGetMaterialString(paiscene->mMaterials[paimesh->mMaterialIndex], AI_MATKEY_NAME, &aiMaterialName);
        MaterialName = aiMaterialName.data;

        // Hadle material name repalcement. 
        auto it = configfile.MaterialReplacements.find(MaterialName);
        if (it != configfile.MaterialReplacements.end()) {
            MaterialName = it->second;
        }
        MaterialName = "materials/" + MaterialName;
    }
public:
    ImportedMesh() = delete;
    std::vector<Geometry::Modelfile::GraphicsVertex> Vertices;
    std::vector<uint32_t> Indices;
    float Radius = 0.0f;
    std::string MaterialName;
};

class ImportedModel {
public:
    std::vector<ImportedMesh> Meshes;

    ImportedModel(std::string path) {
        const aiScene* scene = MakeAssimpScene(path);
        
        Meshes.reserve(scene->mNumMeshes);
        for (int meshindex = 0; meshindex < scene->mNumMeshes; meshindex++) {
            ImportedMesh mesh(reinterpret_cast<void*>(scene->mMeshes[meshindex]), reinterpret_cast<const void*>(scene));
            Meshes.push_back(std::move(mesh));
        }
        aiReleaseImport(scene);
    }
};

struct ImportedLOD {
    ImportedModel Model;
    float Distance;

    ImportedLOD(const ImportConfigurationLOD& from) : Model(inpath.parent_path().string() + "/" + from.Model), Distance(from.Distance) {};
};

int main(int argc, char* argv[]) {
    // Reserve a bit of space for speed.
    LODLump.reserve(16);
    MeshLump.reserve(32);
    VertexLump.reserve(100000);
    IndexLump.reserve(100000);
    StringLump.reserve(1024);

    // Process arguments.
    std::vector<char*> args = {argv, argv + argc};
    args.erase(args.begin()); // Because working directory.

    while (!args.empty()) { // TODO: Add help string.
        std::string_view arg = args[0];

        if (arg == "-file") {
            args.erase(args.begin());
            if (args.empty()) {
                Engine::Error("Parameter for -file is missing");
            }

            inpath = std::string(args[0]);
            args.erase(args.begin());
        }

        if (arg == "-out") {
            args.erase(args.begin());
            if (args.empty()) {
                Engine::Error("Parameter for -out is missing");
            }

            outpath = std::string(args[0]) + "/" + inpath.filename().replace_extension("amdl").string();
            args.erase(args.begin());
        }
    }

    ADFEntry::FromFile(inpath)["Model"].Deserialize(configfile);    

    // Flag checking.
    //Flags |=  // (Yeah there's nothing right now.)
    
    // Import the LOD info and files.
    std::vector<ImportedLOD> LODs;
    LODs.reserve(configfile.LODs.size());
    for (const auto& LOD : configfile.LODs) LODs.push_back(LOD);

    if (LODs.empty()) Engine::Error("At least one LOD is required for a model!");

    LODs[0].Distance = -std::numeric_limits<float>::infinity();

    // Collect everything into the lumps.
    for (const auto& LOD : LODs) {
        LODLump_t CurrLOD;
        CurrLOD.MeshOffset = MeshLump.size();
        CurrLOD.MeshCount = LOD.Model.Meshes.size();
        CurrLOD.Distance = LOD.Distance;

        for (const auto& Mesh : LOD.Model.Meshes) {
            MeshLump_t CurrMesh;
            CurrMesh.VertexOffset = VertexLump.size();
            CurrMesh.VertexCount = Mesh.Vertices.size();
            CurrMesh.IndexOffset = IndexLump.size();
            CurrMesh.IndexCount = Mesh.Indices.size();
            CurrMesh.MaterialNameOffset = StringLump.size();
            Radius = std::max(Radius, Mesh.Radius);

            VertexLump.reserve(VertexLump.size() + CurrMesh.VertexCount);
            VertexLump.insert(VertexLump.end(), Mesh.Vertices.begin(), Mesh.Vertices.end());
            IndexLump.reserve(IndexLump.size() + CurrMesh.IndexCount);
            IndexLump.insert(IndexLump.end(), Mesh.Indices.begin(), Mesh.Indices.end());

            StringLump.resize(StringLump.size() + Mesh.MaterialName.length() + 1);
            std::transform(Mesh.MaterialName.begin(), ++Mesh.MaterialName.end(), StringLump.begin() + CurrMesh.MaterialNameOffset, [](char in){
                return Engine::BinaryFileStringLump(in);
            });

            MeshLump.push_back(CurrMesh);
        }

        LODLump.push_back(CurrLOD);
    }

    // Need to handle collision model as well.
    if (configfile.CollisionModel.has_value()) {
        ImportedModel modelfile = {configfile.CollisionModel.value()};
        ImportedMesh& mesh = modelfile.Meshes[0];
        CollisionVertexLump.reserve(mesh.Vertices.size());
        CollisionIndexLump.reserve(mesh.Indices.size());
        for (const auto& vertex : mesh.Vertices) CollisionVertexLump.push_back(vertex.Position);
        for (const auto& index : mesh.Indices) CollisionIndexLump.push_back(index);
    }

    // Finish the file.

    Engine::BinaryFileConstructor<Geometry::Modelfile> fileconstructor;
    auto file = fileconstructor.GetHeader();

    file->Vertices = fileconstructor.CreateLump<GraphicsVertex>(VertexLump);
    file->Indices = fileconstructor.CreateLump<uint32_t>(IndexLump);
    file->Meshes = fileconstructor.CreateLump<MeshLump_t>(MeshLump);
    file->LODs = fileconstructor.CreateLump<LODLump_t>(LODLump);
    file->Strings = fileconstructor.CreateLump<Engine::BinaryFileStringLump>(StringLump);
    if (!CollisionVertexLump.empty()) {
        file->CollisionVertices = fileconstructor.CreateLump<vec3>(CollisionVertexLump);
        file->CollisionIndices = fileconstructor.CreateLump<uint32_t>(CollisionIndexLump);
    }
    file->Flags = Flags;
    file->Radius = Radius;

    std::ofstream outputfile = Engine::GetFileOutputStream(outpath, std::ios::binary);
    if (!outputfile) {
        Engine::Error("Failed to open output for writing!(" + outpath.string() + ")");
    }
    std::span<std::byte> outputspan = fileconstructor.Export();
    outputfile.write(reinterpret_cast<const char*>(outputspan.data()), outputspan.size_bytes());
    outputfile.close();


    Engine::Print("amdl: Work completed!");
}