#include "glengine/ObjLoader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <unordered_map>
#include <tuple>
#include <cctype>


namespace {
struct IndexTriplet { int v=-1, vt=-1, vn=-1; };

struct TripletHash
{
    size_t operator()(const IndexTriplet& t) const noexcept {
        // simple mix
        size_t h = 1469598103934665603ull;
        auto mix = [&](int x){ h ^= std::hash<int>{}(x); h *= 1099511628211ull; };
        mix(t.v); mix(t.vt); mix(t.vn); return h;
    }
};
struct TripletEq
{
    bool operator()(const IndexTriplet& a, const IndexTriplet& b) const noexcept {
        return a.v==b.v && a.vt==b.vt && a.vn==b.vn;
    }
};

static inline bool starts_with(const std::string& s, const char* pfx)
{
    return s.size() >= std::char_traits<char>::length(pfx)
        && std::equal(s.begin(), s.begin()+std::strlen(pfx), pfx);
}

static IndexTriplet parse_element(const std::string& tok)
{
    // token formats: v | v/vt | v//vn | v/vt/vn  (obj is 1-based, negatives allowed)
    IndexTriplet it{};
    int slash1 = -1, slash2 = -1;
    for (int i=0;i<(int)tok.size();++i) if (tok[i]=='/') { if (slash1==-1) slash1=i; else { slash2=i; break; } }
    auto toInt = [&](int beg, int end)->int {
        if (beg>=end) return 0;
        return std::stoi(tok.substr(beg, end-beg));
    };

    if (slash1==-1) {
        it.v = toInt(0, (int)tok.size());
    } else if (slash2==-1) {
        it.v  = toInt(0, slash1);
        it.vt = toInt(slash1+1, (int)tok.size());
    } else {
        it.v  = toInt(0, slash1);
        if (slash2 == slash1+1) { // v//vn
            it.vt = 0;
        } else {
            it.vt = toInt(slash1+1, slash2);
        }
        it.vn = toInt(slash2+1, (int)tok.size());
    }
    return it;
}

// Convert OBJ index (1-based, negatives allowed) to 0-based into array of size N
static int fix_index(int idx, int N)
{
    if (idx > 0)  return idx - 1;
    if (idx < 0)  return N + idx; // -1 => last
    return -1; // 0 means "missing"
}

// static void compute_face_normal(const glm::vec3& a, const glm::vec3& b,
//                                 const glm::vec3& c,
//                                 glm::vec3& na, glm::vec3& nb, glm::vec3& nc)
// {
//     glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
//     na = nb = nc = glm::isnan(n.x) ? glm::vec3(0,0,1) : n;
// }
} // namespace

ObjMesh ObjLoader::load(const std::string& path, bool genNormals)
{
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Failed to open OBJ: " + path);

    std::vector<glm::vec3> positions; positions.reserve(1024);
    std::vector<glm::vec2> uvs;       uvs.reserve(1024);
    std::vector<glm::vec3> normals;   normals.reserve(1024);

    // Final mesh buffers
    ObjMesh mesh;
    mesh.vertices.reserve(2048);
    mesh.indices.reserve(4096);

    std::unordered_map<IndexTriplet, uint32_t, TripletHash, TripletEq> cache;

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0]=='#') continue;

        std::istringstream iss(line);
        std::string head; iss >> head;
        if (head == "v") {
            glm::vec3 p; iss >> p.x >> p.y >> p.z; positions.push_back(p);
        } else if (head == "vt") {
            glm::vec2 t; iss >> t.x >> t.y; uvs.push_back(t);
        } else if (head == "vn") {
            glm::vec3 n; iss >> n.x >> n.y >> n.z; normals.push_back(n);
        } else if (head == "f") {
            std::vector<IndexTriplet> face;
            std::string tok;
            while (iss >> tok) face.push_back(parse_element(tok));
            if (face.size() < 3) continue;

            // Triangulate fan: (0,i,i+1)
            for (size_t i = 1; i + 1 < face.size(); ++i) {
                IndexTriplet tri[3] = { face[0], face[i], face[i+1] };
                // For each corner, build or reuse a vertex
                for (int k=0; k<3; ++k) {
                    IndexTriplet it = tri[k];
                    auto itFixed = IndexTriplet{
                        fix_index(it.v,  (int)positions.size()),
                        fix_index(it.vt, (int)uvs.size()),
                        fix_index(it.vn, (int)normals.size())
                    };
                    auto found = cache.find(itFixed);
                    if (found != cache.end()) {
                        mesh.indices.push_back(found->second);
                    } else {
                        ObjVertex v{};
                        v.pos = positions.at(itFixed.v);
                        if (itFixed.vt >= 0) v.uv = uvs.at(itFixed.vt);
                        if (itFixed.vn >= 0) v.normal = normals.at(itFixed.vn);
                        uint32_t newIndex = (uint32_t)mesh.vertices.size();
                        mesh.vertices.push_back(v);
                        cache.emplace(itFixed, newIndex);
                        mesh.indices.push_back(newIndex);
                    }
                }
            }
        }
        // ignore mtllib/usemtl/s/etc. for this minimal loader
    }

    // Optionally generate normals if missing
    if (genNormals) {
        bool needNormals = false;
        for (const auto& v : mesh.vertices) {
            if (glm::dot(v.normal, v.normal) < 1e-8f) { needNormals = true; break; }
        }
        if (needNormals) {
            std::vector<glm::vec3> acc(mesh.vertices.size(), glm::vec3(0));
            for (size_t i=0; i+2<mesh.indices.size(); i+=3) {
                uint32_t ia = mesh.indices[i];
                uint32_t ib = mesh.indices[i+1];
                uint32_t ic = mesh.indices[i+2];
                
                const glm::vec3& A = mesh.vertices[ia].pos;
                const glm::vec3& B = mesh.vertices[ib].pos;
                const glm::vec3& C = mesh.vertices[ic].pos;
                
                glm::vec3 n = glm::normalize(glm::cross(B-A, C-A));
                if (!glm::any(glm::isnan(n))) {
                    acc[ia]+=n; acc[ib]+=n; acc[ic]+=n;
                }
            }
            for (size_t i=0;i<mesh.vertices.size();++i) {
                glm::vec3 n = glm::normalize(acc[i]);
                if (!glm::any(glm::isnan(n))) mesh.vertices[i].normal = n;
                else mesh.vertices[i].normal = glm::vec3(0,0,1);
            }
        }
    }

    return mesh;
}
