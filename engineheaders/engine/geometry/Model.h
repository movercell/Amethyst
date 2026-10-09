#pragma once

#include "engine/master.h"
#include <vector>
#include <cstdint>
#include <string>

namespace Geometry {

    class ENGINEEXPORT Model;

    class ENGINEEXPORT Mesh {
    protected:
        friend class Model;
        Mesh(void* Meshdata, const void* Scenedata);
    public:
        Mesh() = delete;
        std::vector<Shapes::Vertex> Vertices;
        std::vector<uint32_t> Indices;
        float Radius = 0.0f;
        std::string MaterialName;
    };

    class ENGINEEXPORT Model {
    public:
        std::vector<Mesh> Meshes;
        Model(std::string path);
        Model() {};
    };

}