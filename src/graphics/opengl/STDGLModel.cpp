#include "STDGLCamera.h"
#include "engine/master.h"
#include "STDGLModel.h"
#include "engine/filesystem/ADF.h"

STDGLModel::STDGLModel(std::string path) {
    ModelInfo_t Info;
    auto ModelADFFull = ADFEntry::FromFile("models/" + path);

    if (!ModelADFFull.HasChild("Model")) {
        new (this) STDGLModel("error.adf");
        return;
    }

    const auto& ModelADF = ModelADFFull["Model"];

    if (!ModelADF.HasChildren()) {
        new (this) STDGLModel("error.adf");
        return;
    }

    LODCount = std::min((int)ModelADF["LODs"].GetArray().size(), STDGLMODEL_LOD_MAX_COUNT);

    if (LODCount < 1) {
        new (this) STDGLModel("error.adf");
        return;
    }



    std::array<Geometry::Model, STDGLMODEL_LOD_MAX_COUNT> LODModels;

    for (int LOD = 0; LOD < STDGLMODEL_LOD_MAX_COUNT; LOD++) {
        Info.LODDistances[LOD] = INFINITY;
    }

    // Load the LOD models
    for (int LOD = 0; LOD < LODCount; LOD++) {
        const ADFEntry& LODEntry = ModelADF["LODs"][LOD];
        LODModels[LOD] = Geometry::Model(LODEntry["Model"].GetString());
        LODs[LOD].MeshCount = std::min((int)LODModels[LOD].Meshes.size(), STDGLMODEL_MESH_MAX_COUNT);

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
    glNamedBufferData(VBO, vertices.size() * sizeof(Shapes::Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glNamedBufferData(EBO, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glNamedBufferData(ModelInfo, sizeof(ModelInfo_t), &Info, GL_STATIC_DRAW);

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
    parent->InstanceBufferMapped[!(parent->ShouldUseOtherBuffer)].InstanceMatrices[index] = Matrix;
    parent->wasModified = true;
}

STDGLModelInstance::~STDGLModelInstance() {
    parent->InstanceBufferMapped[0].InstanceMatrices[index][0, 0] = NAN;
    parent->InstanceBufferMapped[1].InstanceMatrices[index][0, 0] = NAN;
    parent->FreedIndices.push(index);

    if (parent->FreedIndices.size() == parent->NextIndex) {
        std::queue<uint16_t> empty;
        parent->FreedIndices.swap(empty);
        parent->NextIndex = 0;
    }
}




STDGLModelInstanceArray::STDGLModelInstanceArray(GLFWwindow* data, Engine::Reference<STDGLModel> model) {
    rendererData = data;
    Model = model;

    glCreateBuffers(1, &InstanceBuffer);
    glNamedBufferStorage(InstanceBuffer, sizeof(InstanceArrayBuffer[2]), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    InstanceBufferMapped = (InstanceArrayBuffer*)glMapNamedBufferRange(InstanceBuffer, 0, sizeof(InstanceArrayBuffer[2]), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    
    for (int i = 0; i < 2; i++) {
        for (auto& instance : InstanceBufferMapped[i].InstanceMatrices) {
            instance[0, 0] = NAN;
        }
    }
    glFlushMappedNamedBufferRange(InstanceBuffer, 0, sizeof(InstanceArrayBuffer[2]));
}

STDGLModelInstanceArray::~STDGLModelInstanceArray() {
    glfwMakeContextCurrent(rendererData);

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
    try {
        return Engine::Reference(Models.at(path));
    } catch(...) {
        auto ModelResource = new Engine::ManagedResource<STDGLModelSystem, STDGLModel>(this, path);
        Models.emplace(path, ModelResource);
        return Engine::Reference(ModelResource);
    }
}
