#include <iostream>

#include <glengine/orbitalCamera.hpp>
#include "glengine/Renderer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void* shift(int n) { return (void*)(n*sizeof(float)); }

void attrib(int loc, int size, GLsizei stride, int nshift) {
    glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, shift(nshift));
    glEnableVertexAttribArray(loc);
}

std::vector<float> generateNormalLines(ObjectMesh& mesh)
{
    std::vector<float> normalLines;
    normalLines.reserve(mesh.vertices.size() * 2 * 3);
    float scale = 0.1f;

    for (const auto& v : mesh.vertices) {
        glm::vec3 p = v.pos;
        glm::vec3 n = glm::normalize(v.normal);
        glm::vec3 q = p + n * scale;

        normalLines.insert(normalLines.end(), {p.x,p.y,p.z, q.x,q.y,q.z});
    }
    return normalLines;
}

Renderer::Renderer(ObjMesh& mesh, ShaderMap& shaderPaths)
{
    // ------------------------------- DATA ------------------------------- //

    auto data = mesh.interleavedPNV();
    mesh_size = mesh.indices.size();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float),
                                            data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
             mesh.indices.size()*sizeof(uint32_t),
             mesh.indices.data(), GL_STATIC_DRAW);

    // 3 positions + 3 colors
    GLsizei stride = 8*sizeof(float);

    attrib(0, 3, stride, 0);
    attrib(1, 3, stride, 3);
    attrib(2, 2, stride, 6);

    // ---------------------------- NORMAL LINES -------------------------- //

    normalLines = generateNormalLines(mesh);
    
    glGenVertexArrays(1, &vaoN);
    glGenBuffers(1, &vboN);

    glBindVertexArray(vaoN);
    glBindBuffer(GL_ARRAY_BUFFER, vboN);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size()*sizeof(float),
                                        normalLines.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // ------------------------------ SHADERS ----------------------------- //
    unsigned vertexShader   =
            handleShader(shaderPaths.at("vertex"), 1, GL_VERTEX_SHADER);
    unsigned geometryShader   =
            handleShader(shaderPaths.at("geometry"), 1, GL_GEOMETRY_SHADER);
    unsigned fragmentShader = 
            handleShader(shaderPaths.at("fragment"), 1, GL_FRAGMENT_SHADER);
    
    std::vector<unsigned> shaders = {vertexShader, fragmentShader};
    mainProgram_ = createProgram(shaders);
    lineProgram_ = createProgram(geometryShader)
}

Renderer::~Renderer()
{
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (mainProgram_) glDeleteProgram(mainProgram_);
}

void Renderer::draw()
{
    // Data
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view  = camera_.getViewMatrix();
    glm::mat4 proj  = glm::perspective(camera_.getFov(),
                                       src_width / (float)src_height,
                                       near_plane, far_plane);
    glm::mat4 mvp   = proj * view * model;
    glm::mat3 uNormalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    int mvpLoc = glGetUniformLocation(mainProgram_, "uMVP");


    // Line program
    glUseProgram(lineProgram);

    glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vaoN);
    
    glLineWidth(1.0f);                 // optional
    glDrawArrays(GL_LINES, 0, (GLsizei)(normalLines.size()/3));

    glBindVertexArray(0);

    // Main program
    glUseProgram(mainProgram_);

    glBindVertexArray(vao_);

    glDrawElements(GL_TRIANGLES, mesh_size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::onMouseButton(int button, int action, int mods)
{
  if (1) { ///// !ImGui::GetIO().WantCaptureMouse) {
    if (action == GLFW_RELEASE) {
      mouseButtonState = MousePressedButton::NONE;
    }
    else {
      switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: mouseButtonState = MousePressedButton::LEFT;
          break;
        case GLFW_MOUSE_BUTTON_RIGHT: mouseButtonState = MousePressedButton::RIGHT;
          break;
        case GLFW_MOUSE_BUTTON_MIDDLE: mouseButtonState = MousePressedButton::MIDDLE;
          break;  
      }
    }
  }
}

void Renderer::onMouseMove(double xpos, double ypos)
{
  if (1) { ///// !ImGui::GetIO().WantCaptureMouse) {
    if (mouseButtonState == MousePressedButton::NONE) {
      lastX = (float)xpos;
      lastY = (float)ypos;
    }
    else {
      if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
      }

      float xoffset = (float)xpos - lastX;
      float yoffset = lastY - (float)ypos;

      lastX = (float)xpos;
      lastY = (float)ypos;

      switch (mouseButtonState) {
        case MousePressedButton::LEFT: camera_.orbit(xoffset, yoffset);
          break;
        case MousePressedButton::RIGHT:
          camera_.track(xoffset);
          camera_.pedestal(yoffset);
          break;
        case MousePressedButton::MIDDLE: camera_.dolly(yoffset);
          break;
      }
    }
  }
}

void Renderer::onMouseScroll(double xoffset, double yoffset)
{
  if (1) { ///// !ImGui::GetIO().WantCaptureMouse) {
    camera_.zoom((float)yoffset);
  }
}