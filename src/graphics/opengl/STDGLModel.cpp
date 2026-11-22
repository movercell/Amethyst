#include "STDGLCamera.h"
#include "GLMisc.h"
#include "engine/master.h"
#include "STDGLModel.h"
#include "engine/Filesystem.h"
#include "engine/Binaryfile.h"
#include "engine/geometry/ModelFile.h"
#include <ranges>

STDGLModel::STDGLModel(std::string path, STDGLMaterialSystem* MaterialSystem) {
    ModelInfo_t Info;
    auto Modelfilestream = Engine::GetFileAsStream("models/" + path, std::ios::binary);
    if (!Modelfilestream) {
        IsBad = true;
        return;
    }

    std::vector<char> buffer;
    Modelfilestream.seekg(0, std::ios::end);
    buffer.resize(Modelfilestream.tellg());
    Modelfilestream.seekg(0, std::ios::beg);
    buffer.assign(std::istreambuf_iterator<char>{Modelfilestream}, {});

    auto& Modelfile = *std::get<Geometry::Modelfile*>(Engine::BinaryFileImporter<Geometry::Modelfile>::Import(std::as_writable_bytes(std::span(buffer)), path));
    
    LODCount = Modelfile.LODs.Get().size();
    Info.Radius = Modelfile.Radius;

    // Need to set unused LODs as infinity in distance.
    for (float& distance : Info.LODDistances) { distance = std::numeric_limits<float>::infinity(); }
    for (int LODID = 0; const auto& FileLOD : Modelfile.LODs.Get()) {
        if (!(LODID < STDGLMODEL_LOD_MAX_COUNT)) {
            Engine::Error("STDGLModel: Model " + path + "exceeded STDGLMODEL_LOD_MAX_COUNT!");
        }

        Info.LODDistances[LODID] = FileLOD.Distance;

        auto& CurrLOD = LODs[LODID];
        CurrLOD.MeshCount = FileLOD.MeshCount;
        if (FileLOD.MeshCount > STDGLMODEL_MESH_MAX_COUNT) {
            Engine::Error("STDGLModel: Model " + path + "exceeded STDGLMODEL_MESH_MAX_COUNT!");
        }
        
        for (int MeshID = 0; MeshID < FileLOD.MeshCount; MeshID++) {
            const auto& FileMesh = Modelfile.Meshes.Get().at(FileLOD.MeshOffset + MeshID);
            CurrLOD.Meshes[MeshID].Material = MaterialSystem->Get(reinterpret_cast<char*>(&Modelfile.Strings.Get().at(FileMesh.MaterialNameOffset)));

            auto& DrawData = IndirectBufferTemplates[LODID][MeshID];
            DrawData.baseVertex = FileMesh.VertexOffset;
            DrawData.firstIndex = FileMesh.IndexOffset;
            DrawData.count = FileMesh.IndexCount;
            DrawData.baseInstance = LODID;
        }

        LODID++;
    }
    
    // Upload to the GPU.

    auto vertices = Modelfile.Vertices.Get();
    auto indices = Modelfile.Indices.Get();

    glCreateBuffers(3, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glNamedBufferStorage(VBO, vertices.size() * sizeof(Geometry::Modelfile::GraphicsVertex), vertices.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glNamedBufferStorage(EBO, indices.size() * sizeof(GLuint), indices.data(), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glNamedBufferStorage(ModelInfo, sizeof(ModelInfo_t), &Info, 0);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Modelfile::GraphicsVertex), (void*)offsetof(Geometry::Modelfile::GraphicsVertex, Position));
    // Vertex normals.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Modelfile::GraphicsVertex), (void*)offsetof(Geometry::Modelfile::GraphicsVertex, Normal));
    // Vertex texture coordinates.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Geometry::Modelfile::GraphicsVertex), (void*)offsetof(Geometry::Modelfile::GraphicsVertex, TexCoords));
    // Vertex tangent space.
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Geometry::Modelfile::GraphicsVertex), (void*)offsetof(Geometry::Modelfile::GraphicsVertex, Tangent));
}
STDGLModel::~STDGLModel() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, &VBO);
}




void STDGLModelInstance::SetMatrix(mat4 Matrix) {
    parent->InstanceStagingBufferMapped[parent->Context->FrameID].InstanceMatrices[index] = Matrix;
}

STDGLModelInstance::~STDGLModelInstance() {
    parent->InstanceStagingBufferMapped[0].InstanceMatrices[index][0, 0] = NAN;
    parent->InstanceStagingBufferMapped[1].InstanceMatrices[index][0, 0] = NAN;
    parent->FreedIndices.push(index);

    if (parent->FreedIndices.size() == parent->NextIndex) {
        std::queue<uint16_t> empty;
        parent->FreedIndices.swap(empty);
        parent->NextIndex = 0;
    }
}




STDGLModelInstanceArray::STDGLModelInstanceArray(GLContext* context, Engine::Reference<STDGLModel> model) {
    Context = context;
    Model = model;

    glCreateBuffers(1, &InstanceStagingBuffer);
    glNamedBufferStorage(InstanceStagingBuffer, sizeof(InstanceArray[2]), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    InstanceStagingBufferMapped = (InstanceArray*)glMapNamedBufferRange(InstanceStagingBuffer, 0, sizeof(InstanceArray[2]), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    glCreateBuffers(1, &InstanceBuffer);
    glNamedBufferStorage(InstanceBuffer, sizeof(InstanceArrayBuffer), NULL, GL_DYNAMIC_STORAGE_BIT);
    CopyDrawDataFromModel();
    
    for (int i = 0; i < 2; i++) {
        for (auto& instance : InstanceStagingBufferMapped[i].InstanceMatrices) {
            instance[0, 0] = NAN;
        }
    }
    glFlushMappedNamedBufferRange(InstanceStagingBuffer, 0, sizeof(InstanceArray[2]));
}

STDGLModelInstanceArray::~STDGLModelInstanceArray() {
    GLMisc::SetContext(Context);

    glDeleteBuffers(1, &InstanceStagingBuffer);
    glDeleteBuffers(1, &InstanceBuffer);
}

std::unique_ptr<ModelInstance> STDGLModelInstanceArray::MakeModelInstance() {
    // Because there can be multiple of them one needs to check whether the current one has enough space;
    if (FreedIndices.empty() && NextIndex == STDGLMODEL_INSTANCE_MAX_COUNT) {
        return nullptr;
    }
    
    uint16_t index;
    if (FreedIndices.empty()) {
        index = NextIndex;
        NextIndex++;
    } else {
        index = FreedIndices.front();
        FreedIndices.pop();
    }
    return std::make_unique<STDGLModelInstance>(index, selfResource);
}



Engine::Reference<STDGLModel> STDGLModelSystem::GetModel(std::string path) {
    auto Model = Models.find(path);
    
    if (Model != Models.end()) {
        return Engine::Reference(Model->second);
    } else {
        auto ModelResource = new Engine::ManagedResource<STDGLModelSystem, STDGLModel>(this, path, MaterialSystem);

        if (ModelResource->resource.IsBad) {
            delete ModelResource;
            ModelResource = new Engine::ManagedResource<STDGLModelSystem, STDGLModel>(this, "error.amdl", MaterialSystem);

            if (ModelResource->resource.IsBad) {
                Engine::Error("Error loading the error model!");
            }
        }

        Models.emplace(path, ModelResource);
        return Engine::Reference(ModelResource);
    }
}
