#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

struct ObjVertex {
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 uv{};
};

struct ObjMesh {
    std::vector<ObjVertex> vertices;   // deduplicated vertex buffer
    std::vector<uint32_t>  indices;    // index buffer (triangles)

    // Optional: build interleaved float buffer (pos, normal, uv) for VBOs
    std::vector<float> interleavedPNV() const {
        std::vector<float> out; out.reserve(vertices.size()*8);
        for (const auto& v : vertices) {
            out.push_back(v.pos.x);
            out.push_back(v.pos.y);
            out.push_back(v.pos.z);

            out.push_back(v.normal.x);
            out.push_back(v.normal.y);
            out.push_back(v.normal.z);

            out.push_back(v.uv.x);
            out.push_back(v.uv.y);
        }
        return out;
    }
};

class ObjLoader {
public:
    // Throws std::runtime_error on failure.
    static ObjMesh load(const string& path, bool computeMissingNormals = true);
};
