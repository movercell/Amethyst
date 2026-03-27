#include "STDGLCamera.h"
#include "engine/master.h"
#include "STDGLModel.h"

STDGLModel::STDGLModel(std::string path = "error.glb") {
    Geometry::Model model(path);
    Path = path;

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
        Info.IndirectBufferTemplates[0][meshindex].firstIndex = mesh_base_index;
        Info.IndirectBufferTemplates[0][meshindex].baseVertex = mesh_base_vertex;
        Info.IndirectBufferTemplates[0][meshindex].count      = (unsigned int)mesh.Indeces.size();

        Meshes[0][meshindex] = Mesh();

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


void STDGLModel::Bind() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ModelInfo);
    glBindVertexArray(VAO);
}

void STDGLModel::Draw() {
    for (int mesh = 0; mesh < MeshCount; mesh++) {
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(sizeof(DrawElementsIndirectCommand) * mesh));
    }
}




void STDGLModelInstance::SetMatrix(mat4 Matrix) {
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4) + offsetof(STDGLModelInstanceArray::InstanceArrayBuffer, InstanceMatrices), sizeof(mat4), &Matrix);
}

STDGLModelInstance::~STDGLModelInstance() {
    float temp = NAN;
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4) + offsetof(STDGLModelInstanceArray::InstanceArrayBuffer, InstanceMatrices), sizeof(float), &temp);

    parent->FreedIndeces.push(index);
}




STDGLModelInstanceArray::STDGLModelInstanceArray(GLFWwindow* data, std::shared_ptr<STDGLModel> model) {
    rendererData = data;
    Model = model;

    glCreateBuffers(1, &InstanceBuffer);
    std::unique_ptr<InstanceArrayBuffer> Buffer = std::make_unique<InstanceArrayBuffer>();
    std::fill(Buffer->InstanceMatrices, Buffer->InstanceMatrices + STDGLMODEL_INSTANCE_MAX_COUNT * sizeof(mat4), 0xFF);
    glNamedBufferData(InstanceBuffer, sizeof(InstanceArrayBuffer), Buffer.get(), GL_DYNAMIC_DRAW);
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


void STDGLModelInstanceArray::Bind() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstanceBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, InstanceBuffer);
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
