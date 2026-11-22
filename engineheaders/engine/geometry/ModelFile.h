#pragma once

#include "engine/master.h"
#include "engine/Binaryfile.h"

namespace Geometry {
    struct BinaryFile("AMDL", 0) Modelfile : Engine::BinaryFileBaseHeader {
        struct UnalignedVec3 {
            std::array<float, 3> data;

            UnalignedVec3(vec3 from = vec3()) { data[0] = from.x; data[1] = from.y; data[2] = from.z;}
            operator vec3() { return {data[0], data[1], data[2]}; }
        };
        struct GraphicsVertex {
            // To get bitangent you will need to multiply the cross product of normal and tangent by the tangent's W.
            vec4 Tangent;
            UnalignedVec3 Position;
            UnalignedVec3 Normal;
            vec2 TexCoords;
        };

        struct MeshLump {
            uint32_t VertexOffset = 0;
            uint32_t VertexCount = 0;
            uint32_t IndexOffset = 0;
            uint32_t IndexCount = 0;
            uint32_t MaterialNameOffset = 0;
        };

        struct LODLump {
            uint32_t MeshOffset = 0;
            uint32_t MeshCount = 0;
            float Distance = 0.0f;
        };

        Engine::BinaryFileLump<GraphicsVertex> Vertices;
        Engine::BinaryFileLump<uint32_t> Indices;
        Engine::BinaryFileLump<MeshLump> Meshes;
        Engine::BinaryFileLump<LODLump> LODs;
        Engine::BinaryFileLump<Engine::BinaryFileStringLump> Strings;
        Engine::BinaryFileLump<vec3> CollisionVertices;
        Engine::BinaryFileLump<uint32_t> CollisionIndices;

        enum class FlagDefs : uint64_t {
            None = 0,
        };

        uint64_t Flags = 0;
        float Radius = 0.0f;
    };
}