#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct ObjMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Returns interleaved [pos, normal, uv]
    std::vector<float> interleavedPNV() const {
        std::vector<float> data;
        data.reserve(vertices.size() * 8);
        for (const auto& v : vertices) {
            data.push_back(v.pos.x);
            data.push_back(v.pos.y);
            data.push_back(v.pos.z);

            data.push_back(v.normal.x);
            data.push_back(v.normal.y);
            data.push_back(v.normal.z);

            data.push_back(v.uv.x);
            data.push_back(v.uv.y);
        }
        return data;
    }
};

class ObjLoader {
public:
    static ObjMesh load(const std::string& path);
    static void generateNormals(ObjMesh& mesh);
};

#endif
