#include <iostream>

#include <glengine/ObjLoader.hpp>
#include <glengine/orbitalCamera.hpp>
#include <glengine/Renderer.hpp>
#include <glengine/Logger.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define DrawLines

static void* shift(int n) { return (void*)(n*sizeof(float)); }

static void attrib(int loc, int size, GLsizei stride, int nshift) {
    glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, shift(nshift));
    glEnableVertexAttribArray(loc);
}

Renderer::Renderer(ObjMesh& mesh, ShaderMap& shaderPaths)
    : lastX(0), lastY(0)
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

    // ------------------------------ SHADERS ----------------------------- //
    unsigned mainVertexShader   =
            handleShader(shaderPaths.at("main_vertex"), 1, GL_VERTEX_SHADER);
    unsigned mainFragmentShader = 
            handleShader(shaderPaths.at("main_fragment"), 1, GL_FRAGMENT_SHADER);
    
    std::vector<unsigned> mainShaders = {
        mainVertexShader,
        mainFragmentShader
    };
    mainProgram_ = createProgram(mainShaders);
    
    unsigned lineVertexShader =
        handleShader(shaderPaths.at("line_vertex"), 1, GL_VERTEX_SHADER);
    unsigned lineGeometryShader =
        handleShader(shaderPaths.at("line_geometry"), 1, GL_GEOMETRY_SHADER);
    unsigned lineFragmentShader =
        handleShader(shaderPaths.at("line_fragment"), 1, GL_FRAGMENT_SHADER);

    std::vector<unsigned> lineShaders = {
        lineVertexShader,
        lineGeometryShader, 
        lineFragmentShader
    };
    lineProgram_ = createProgram(lineShaders);
}

Renderer::~Renderer()
{
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (mainProgram_) glDeleteProgram(mainProgram_);
    if (lineProgram_) glDeleteProgram(lineProgram_);
}

void Renderer::draw()
{
    // Data
    Logger::log("Renderer::draw - Initializing Data");
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    glm::mat4 view  = camera_.getViewMatrix();
    glm::mat4 proj  = glm::perspective(camera_.getFov(),
                                       src_width / (float)src_height,
                                       near_plane, far_plane);
    glm::mat4 mvp   = proj * view * model;
    glm::mat3 uNormalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    glm::vec3 uCameraPos = camera_.getPosition();

    glm::vec3 lightDir = -glm::vec3(1.6, -1.8, 1.8);
    glm::vec3 lightA = glm::vec3(0.2);
    glm::vec3 lightD = glm::vec3(0.5, 0.5, 0.7);
    glm::vec3 lightS = glm::vec3(0.5);

    #ifdef DrawLines
    // Line program
    {
        Logger::log("Renderer::draw - Setting up line program");

        glUseProgram(lineProgram_);

        int mvpLoc = glGetUniformLocation(lineProgram_, "uMVP");
        int modelLoc = glGetUniformLocation(lineProgram_, "model");
        int viewLoc = glGetUniformLocation(lineProgram_, "view");
        int projLoc = glGetUniformLocation(lineProgram_, "proj");
        int lineLengthLoc = glGetUniformLocation(lineProgram_, "normalLength");

        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glUniform1f(lineLengthLoc, 0.1);

        glLineWidth(0.3f);          // optional

        glBindVertexArray(vao_);

        Logger::log("Renderer::draw - Using line program");
        glDrawElements(GL_TRIANGLES, mesh_size, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
    #endif
    // Main program
    {
        Logger::log("Renderer::draw - Setting up main program");

        glUseProgram(mainProgram_);

        glUniformMatrix4fv(glGetUniformLocation(mainProgram_, "uMVP"),
                    1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(mainProgram_, "uModel"),
                    1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(mainProgram_, "uView"),
                    1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix3fv(glGetUniformLocation(mainProgram_, "uNormalMatrix"),
                    1, GL_FALSE, glm::value_ptr(uNormalMatrix));
        glUniform3fv(glGetUniformLocation(mainProgram_, "uCameraPos"),
                    1, glm::value_ptr(uCameraPos));
        glUniform3fv(glGetUniformLocation(mainProgram_, "uLightDir"),
                    1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(mainProgram_, "uA"),
                    1, glm::value_ptr(lightA));
        glUniform3fv(glGetUniformLocation(mainProgram_, "uD"),
                    1, glm::value_ptr(lightD));
        glUniform3fv(glGetUniformLocation(mainProgram_, "uS"),
                    1, glm::value_ptr(lightS));

        glBindVertexArray(vao_);

        Logger::log("Renderer::draw - Using main program");
        glDrawElements(GL_TRIANGLES, mesh_size, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    Logger::log("Renderer::draw - End");
    Logger::disable();
}

void Renderer::onMouseButton(int button, int action, int mods)
{
    if (1) { ///// !ImGui::GetIO().WantCaptureMouse) {
        if (action == GLFW_RELEASE) {
            mouseButtonState = MousePressedButton::NONE;
        }
        else {
            switch (button) {
               case GLFW_MOUSE_BUTTON_LEFT:
                   mouseButtonState = MousePressedButton::LEFT;
                   break;
               case GLFW_MOUSE_BUTTON_RIGHT:
                   mouseButtonState = MousePressedButton::RIGHT;
                   break;
               case GLFW_MOUSE_BUTTON_MIDDLE:
                   mouseButtonState = MousePressedButton::MIDDLE;
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