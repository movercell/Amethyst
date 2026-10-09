#include "STDGLCamera.h"
#include "GLMisc.h"
#include "engine/master.h"
#include "STDGLModel.h"
#include "engine/filesystem/ADF.h"

STDGLModel::STDGLModel(std::string path, STDGLMaterialSystem* MaterialSystem) {
    ModelInfo_t Info;
    auto ModelADFFull = ADFEntry::FromFile("models/" + path);

    if (!ModelADFFull.HasChild("Model")) {
        new (this) STDGLModel("error.adf", MaterialSystem);
        return;
    }

    const auto& ModelADF = ModelADFFull["Model"];

    if (!ModelADF.HasChild("LODs") || !ModelADF.HasChild("Materials")) {
        new (this) STDGLModel("error.adf", MaterialSystem);
        return;
    }

    const auto& LODsADF = ModelADF["LODs"];
    const auto& MaterialsADF = ModelADF["Materials"];

    LODCount = std::min((int)LODsADF.GetArray().size(), STDGLMODEL_LOD_MAX_COUNT);

    if (LODCount < 1) {
        new (this) STDGLModel("error.adf", MaterialSystem);
        return;
    }



    std::array<Geometry::Model, STDGLMODEL_LOD_MAX_COUNT> LODModels;

    for (int LOD = 0; LOD < STDGLMODEL_LOD_MAX_COUNT; LOD++) {
        Info.LODDistances[LOD] = INFINITY;
    }

    // Load the LOD models
    for (int LOD = 0; LOD < LODCount; LOD++) {
        const ADFEntry& LODEntry = LODsADF[LOD];
        LODModels[LOD] = Geometry::Model(LODEntry["Model"].GetString());
        LODs[LOD].MeshCount = std::min((int)LODModels[LOD].Meshes.size(), STDGLMODEL_MESH_MAX_COUNT);

        // Collect mesh materials.
        for (int meshindex = 0; meshindex < LODs[LOD].MeshCount; meshindex++) {
            std::string MaterialNameInModel = LODModels[LOD].Meshes[meshindex].MaterialName;
            std::string MaterialName = "materials/";
            if (MaterialsADF.HasChild(MaterialNameInModel)) {
                MaterialName += MaterialsADF[MaterialNameInModel].GetString();
            } else {
                MaterialName += MaterialNameInModel;
            }
            
            LODs[LOD].Meshes[meshindex].Material = MaterialSystem->Get(MaterialName);
        }

        float possibledistance = INFINITY;

        if (LODEntry.HasChild("Distance")) {
            possibledistance = std::stof(LODEntry["Distance"].GetString());
        }

        Info.LODDistances[LOD] = (LOD > 0) ? possibledistance : -INFINITY; // LOD 0 must always be distance -INFINITY so that no distance is less than it
    }

    glCreateBuffers(3, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::vector<Shapes::Vertex> vertices;
    std::vector<GLuint> indices;

    {   // Reserve the space
        int vertex_count_total = 0;
        int index_count_total = 0;
        for (int LOD = 0; LOD < LODCount; LOD++) {
            for (const auto& mesh : LODModels[LOD].Meshes) {
                vertex_count_total += mesh.Vertices.size();
                index_count_total += mesh.Indices.size();
            }
        }
        vertices.reserve(vertex_count_total);
        indices.reserve(index_count_total);
    }

    int mesh_base_vertex = 0;
    int mesh_base_index = 0;

    for (int LOD = 0; LOD < LODCount; LOD++) {
        for (int meshindex = 0; meshindex < LODs[LOD].MeshCount; meshindex++) {
            const auto& mesh = LODModels[LOD].Meshes[meshindex];

            Info.IndirectBuffers[LOD][meshindex].count        = (unsigned int)mesh.Indices.size();
            Info.IndirectBuffers[LOD][meshindex].firstIndex   = mesh_base_index;
            Info.IndirectBuffers[LOD][meshindex].baseVertex   = mesh_base_vertex;
            Info.IndirectBuffers[LOD][meshindex].baseInstance = LOD;

            // Concatenate the vectors
            std::copy(mesh.Vertices.cbegin(), mesh.Vertices.cend(), std::back_inserter(vertices));
            std::copy(mesh.Indices.cbegin(),  mesh.Indices.cend(),  std::back_inserter(indices));

            Info.Radius = std::max(Info.Radius, mesh.Radius);

            mesh_base_vertex += mesh.Vertices.size();
            mesh_base_index  += mesh.Indices.size();
        }
    }
    
    // Upload to the GPU
    glNamedBufferStorage(VBO, vertices.size() * sizeof(Shapes::Vertex), vertices.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glNamedBufferStorage(EBO, indices.size() * sizeof(GLuint), indices.data(), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glNamedBufferStorage(ModelInfo, sizeof(ModelInfo_t), &Info, 0);

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
    glNamedBufferStorage(InstanceBuffer, sizeof(InstanceArrayBuffer), NULL, 0);

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
    uint16_t index;
    if (FreedIndices.empty()) {
        index = NextIndex;
        NextIndex++;
    } else {
        index = FreedIndices.front();
        FreedIndices.pop();
    }
    if (index >= STDGLMODEL_INSTANCE_MAX_COUNT)
        Engine::Error("Attempted to create more than STDGLMODEL_INSTANCE_MAX_COUNT instances of the same model!");
    return std::make_unique<STDGLModelInstance>(index, selfResource);
}



Engine::Reference<STDGLModel> STDGLModelSystem::GetModel(std::string path) {
    auto Model = Models.find(path);
    
    if (Model != Models.end()) {
        return Engine::Reference(Model->second);
    } else {
        auto ModelResource = new Engine::ManagedResource<STDGLModelSystem, STDGLModel>(this, path, MaterialSystem);
        Models.emplace(path, ModelResource);
        return Engine::Reference(ModelResource);
    }
}
