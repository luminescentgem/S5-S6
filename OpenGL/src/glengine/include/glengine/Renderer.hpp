#pragma once
#include <vector>
#include <map>

#include "glengine/ObjLoader.hpp"
#include "glengine/Shader.hpp"
#include "glengine/orbitalCamera.hpp"


using ShaderMap = std::map<std::string, std::string>;

class Renderer
{
private:
    unsigned vao_ = 0;
    unsigned vbo_ = 0;
    unsigned ebo_ = 0;

    unsigned mainProgram_ = 0;
    unsigned lineProgram_ = 0;

    unsigned mesh_size;

    unsigned src_width = 800;
    unsigned src_height = 600;

    const float near_plane = 0.1f;
    const float far_plane = 10.0f;

    bool firstMouse = true;
    float lastX; float lastY;

    enum class MousePressedButton { NONE, LEFT, RIGHT, MIDDLE };
    MousePressedButton mouseButtonState = MousePressedButton::NONE;

    GLEngine::OrbitalCamera camera_ = {glm::vec3(0.3f, 0.4f, 3.0f),
                                          glm::vec3(0.0, 0.0, 0.0),
                                          glm::vec3(0.0, 1.0, 0.0)};

    
public:
    Renderer(ObjMesh& mesh, ShaderMap& shaderPaths);
    ~Renderer();
    
    void onMouseButton(int button, int action, int mods);
    void onMouseMove(double xpos, double ypos);
    void onMouseScroll(double xoffset, double yoffset);


    void draw();
};