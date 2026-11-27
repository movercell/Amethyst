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
            // XYZ is tangent, to get the bitangent you will need to multiply the cross product of normal and tangent by the W component.
            vec4 TangentSpace;
            UnalignedVec3 Position;
            UnalignedVec3 Normal;
            vec2 TexCoords;
        };

        struct MeshLump_t {
            uint32_t IndexOffset = 0;
            uint32_t IndexCount = 0;
            uint32_t MaterialNameOffset = 0;
        };

        struct LODLump_t {
            uint32_t MeshOffset = 0;
            uint32_t MeshCount = 0;
            float Distance = 0.0f;
        };

        Engine::BinaryFileLump<GraphicsVertex> VertexLump;
        // The indices lump refers to the full indices, no base vertex is needed.
        Engine::BinaryFileLump<uint32_t> IndexLump;
        Engine::BinaryFileLump<MeshLump_t> MeshLump;
        Engine::BinaryFileLump<LODLump_t> LODLump;
        Engine::BinaryFileLump<Engine::BinaryFileStringLump> StringLump;
        Engine::BinaryFileLump<vec3> CollisionVertexLump;
        Engine::BinaryFileLump<uint32_t> CollisionIndexLump;

        enum class FlagDefs : uint32_t {
            
        };

        uint32_t Flags = 0;
        float Radius = 0.0f;
    };
}