#include <glengine/ObjLoader.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <glm/gtc/constants.hpp>

namespace {
    struct VertexKey {
        int posIdx;
        int uvIdx;
        int normIdx;
        bool operator==(const VertexKey& other) const = default;
    };
    struct VertexKeyHash {
        std::size_t operator()(const VertexKey& k) const noexcept {
            return ((std::hash<int>()(k.posIdx) ^ (std::hash<int>()(k.uvIdx) << 1)) >> 1)
                ^ (std::hash<int>()(k.normIdx) << 1);
        }
    };
}

ObjMesh ObjLoader::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open OBJ file: " << path << std::endl;
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::unordered_map<VertexKey, uint32_t, VertexKeyHash> uniqueVertices;

    ObjMesh mesh;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y; // flip Y for OpenGL
            uvs.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (prefix == "f") {
            std::string vertDesc;
            std::vector<uint32_t> faceIndices;

            while (iss >> vertDesc) {
                int vi = 0, ti = 0, ni = 0;
                char slash;
                std::replace(vertDesc.begin(), vertDesc.end(), '/', ' ');
                std::istringstream vss(vertDesc);
                vss >> vi >> ti >> ni;

                VertexKey key = { vi - 1, ti - 1, ni - 1 };
                auto it = uniqueVertices.find(key);
                uint32_t index;

                if (it == uniqueVertices.end()) {
                    Vertex vert{};
                    vert.pos = positions[key.posIdx];
                    vert.uv = (key.uvIdx >= 0 && key.uvIdx < (int)uvs.size()) ? uvs[key.uvIdx] : glm::vec2(0);
                    vert.normal = (key.normIdx >= 0 && key.normIdx < (int)normals.size()) ? normals[key.normIdx] : glm::vec3(0, 0, 0);

                    index = static_cast<uint32_t>(mesh.vertices.size());
                    mesh.vertices.push_back(vert);
                    uniqueVertices[key] = index;
                }
                else {
                    index = it->second;
                }
                faceIndices.push_back(index);
            }

            // triangulate polygons (fan method)
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                mesh.indices.push_back(faceIndices[0]);
                mesh.indices.push_back(faceIndices[i]);
                mesh.indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    // If normals are missing, generate them
    bool missingNormals = false;
    for (const auto& v : mesh.vertices)
        if (glm::length(v.normal) < 1e-6f)
            missingNormals = true;

    if (missingNormals)
        generateNormals(mesh);

    return mesh;
}

void ObjLoader::generateNormals(ObjMesh& mesh)
{
    std::vector<glm::vec3> acc(mesh.vertices.size(), glm::vec3(0));

    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
        uint32_t ia = mesh.indices[i];
        uint32_t ib = mesh.indices[i + 1];
        uint32_t ic = mesh.indices[i + 2];

        const glm::vec3& A = mesh.vertices[ia].pos;
        const glm::vec3& B = mesh.vertices[ib].pos;
        const glm::vec3& C = mesh.vertices[ic].pos;

        glm::vec3 face = glm::cross(B - A, C - A); // CCW winding
        acc[ia] += face;
        acc[ib] += face;
        acc[ic] += face;
    }

    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        glm::vec3 n = glm::normalize(acc[i]);
        if (glm::any(glm::isnan(n))) n = glm::vec3(0, 0, 1);
        mesh.vertices[i].normal = n;
    }
}
