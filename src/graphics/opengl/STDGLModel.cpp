#include "STDGLCamera.h"
#include "engine/master.h"
#include "STDGLModel.h"

STDGLModel::STDGLModel(std::string path = "error.glb") {
    Geometry::Model model(path);
    Path = path;

    LODCount = 1;
    MeshCount = std::min((int)model.Meshes.size(), STDGLMODEL_MESH_MAX_COUNT);

    ModelInfo_t Info;
    glCreateBuffers(3, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::vector<Shapes::Vertex> vertices;
    std::vector<GLuint> indeces;

    {   // Reserve the space
        int vertex_count_total = 0;
        int index_count_total = 0;
        for (auto mesh : model.Meshes) {
            vertex_count_total += mesh.Vertices.size();
            index_count_total += mesh.Indeces.size();
        }
        vertices.reserve(vertex_count_total);
        indeces.reserve(index_count_total);
    }

    int mesh_base_vertex = 0;
    int mesh_base_index = 0;

    for (int meshindex = 0; meshindex < MeshCount; meshindex++) {
        const auto& mesh = model.Meshes[meshindex];

        Meshes[0][meshindex] = Mesh((unsigned int)mesh.Indeces.size(), mesh_base_vertex, mesh_base_index);

        // Concatenate the vectors
        std::copy(mesh.Vertices.cbegin(), mesh.Vertices.cend(), std::back_inserter(vertices));
        std::copy(mesh.Indeces.cbegin(),  mesh.Indeces.cend(),  std::back_inserter(indeces));

        Info.Radius = std::max(Info.Radius, mesh.Radius);

        mesh_base_vertex += mesh.Vertices.size();
        mesh_base_index  += mesh.Indeces.size();
    }
    
    // Upload to the GPU
    glNamedBufferData(VBO, vertices.size() * sizeof(Shapes::Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glNamedBufferData(EBO, indeces.size() * sizeof(GLuint), indeces.data(), GL_STATIC_DRAW);
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
    glfwMakeContextCurrent(parent->rendererData);

    parent->InstanceBufferMapped->InstanceMatrices[index] = Matrix;
}

STDGLModelInstance::~STDGLModelInstance() {
    float temp = NAN;
    glfwMakeContextCurrent(parent->rendererData);

    parent->InstanceBufferMapped->InstanceMatrices[index][0, 0] = NAN;
    parent->FreedIndeces.push(index);
}




STDGLModelInstanceArray::STDGLModelInstanceArray(GLFWwindow* data, std::shared_ptr<STDGLModel> model) {
    rendererData = data;
    Model = model;

    glCreateBuffers(1, &InstanceBuffer);
    glNamedBufferStorage(InstanceBuffer, sizeof(InstanceArrayBuffer), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    InstanceBufferMapped = (InstanceArrayBuffer*)glMapNamedBufferRange(InstanceBuffer, 0, offsetof(InstanceArrayBuffer, InstanceIndeces), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

    for (auto& instance : InstanceBufferMapped->InstanceMatrices) {
        instance[0, 0] = NAN;
    }

    for (int LOD = 0; LOD < Model->LODCount; LOD++) {
        for (int Mesh = 0; Mesh < Model->MeshCount; Mesh++) {
            InstanceBufferMapped->IndirectBuffers[LOD][Mesh].count      = Model->Meshes[LOD][Mesh].IndexCount;
            InstanceBufferMapped->IndirectBuffers[LOD][Mesh].firstIndex = Model->Meshes[LOD][Mesh].BaseIndex;
            InstanceBufferMapped->IndirectBuffers[LOD][Mesh].baseVertex = Model->Meshes[LOD][Mesh].BaseVertex;
        }
    }

    glFlushMappedNamedBufferRange(InstanceBuffer, 0, offsetof(InstanceArrayBuffer, InstanceIndeces));
}

STDGLModelInstanceArray::~STDGLModelInstanceArray() {
    glfwMakeContextCurrent(rendererData);

    glDeleteBuffers(1, &InstanceBuffer);
}

std::unique_ptr<ModelInstance> STDGLModelInstanceArray::MakeModelInstance() {
    uint16_t index;
    if (FreedIndeces.empty()) {
        index = NextIndex;
        NextIndex++;
    } else {
        index = FreedIndeces.front();
        FreedIndeces.pop();
    }
    if (index >= STDGLMODEL_INSTANCE_MAX_COUNT)
        exit(-1);
    return std::make_unique<STDGLModelInstance>(index, selfRef.lock());
}





std::shared_ptr<STDGLModel> STDGLModelSystem::GetModel(std::string path) {
    auto sharedModels = Models.lock();

    for (auto& model : sharedModels) {
        if (model->Path == path) { return model; }
    }

    std::shared_ptr<STDGLModel> ret = std::make_shared<STDGLModel>(path);
    Models.push_back(ret);
    return ret;
}
