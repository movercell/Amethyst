#include "STDGLCamera.h"
#include "engine/master.h"
#include "STDGLModel.h"

STDGLModel::STDGLModel(std::string path = "error.glb") {
    Geometry::Model model(path);
    Path = path;

    int meshcount = std::min((int)model.Meshes.size(), STDGLMODEL_MESH_MAX_COUNT);

    Meshes.reserve(meshcount);

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

    // Concatenate the vectors
    for (int meshindex = 0; meshindex < meshcount; meshindex++) {
        const auto& mesh = model.Meshes[meshindex];
        Meshes.emplace_back((unsigned int)mesh.Indeces.size(), mesh_base_vertex, mesh_base_index);

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

void STDGLModel::Draw() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ModelInfo);
    glBindVertexArray(VAO);
    for (auto mesh : Meshes) {
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, (void*)(mesh.BaseIndex * sizeof(GLuint)), 4096, mesh.BaseVertex);
    }
}




void STDGLModelInstance::SetMatrix(mat4 Matrix) {
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), &Matrix);
}

STDGLModelInstance::~STDGLModelInstance() {
    mat4 temp = mat4(NAN);
    glfwMakeContextCurrent(parent->rendererData);

    glNamedBufferSubData(parent->InstanceBuffer, index * sizeof(mat4), sizeof(mat4), &temp);

    parent->FreedIndeces.push(index);
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
    return std::make_unique<STDGLModelInstance>(index, selfRef.lock());
}

void STDGLModelInstanceArray::Draw() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstanceBuffer);
    Model->Draw();
}

STDGLModelInstanceArray::STDGLModelInstanceArray(GLFWwindow* data, std::shared_ptr<STDGLModel> model, uint16_t instancemaxcount) {
    rendererData = data;
    InstanceMaxCount = instancemaxcount;
    Model = model;

    glCreateBuffers(1, &InstanceBuffer);
    uint8_t* temparr = new uint8_t[InstanceMaxCount * sizeof(mat4)]; // To init buffer to all NaN
    std::fill(temparr, temparr + InstanceMaxCount * sizeof(mat4), 0xFF);
    glNamedBufferData(InstanceBuffer, InstanceMaxCount * sizeof(mat4), temparr, GL_DYNAMIC_DRAW);
    delete[] temparr;
}

STDGLModelInstanceArray::~STDGLModelInstanceArray() {
    glfwMakeContextCurrent(rendererData);

    glDeleteBuffers(1, &InstanceBuffer);
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