#include "glengine/Window.hpp"
#include "glengine/Renderer.hpp"
#include "glengine/ObjLoader.hpp"
#include "tp02/config.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>


using ShaderMap = std::map<std::string, std::string>;

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    Window window(1600, 1200, "Rectangle!");

    std::string modelPath = full_path("models/dragon2_small.obj");
    ObjMesh mesh = ObjLoader::load(modelPath);

    ShaderMap shaderPaths = {
        {"main_vertex",     full_path("shaders/main.vert")},
        {"main_fragment",   full_path("shaders/npr.frag")},
        {"line_vertex",     full_path("shaders/line.vert")},
        {"line_geometry",   full_path("shaders/line.geom")},
        {"line_fragment",   full_path("shaders/line.frag")}
    };

    Renderer renderer(mesh, shaderPaths);

    std::cout << "Render succesful." <<std::endl;

    glfwSetWindowUserPointer(window.raw(), &renderer);

    glfwSetMouseButtonCallback(window.raw(),
        [](GLFWwindow* w, int button, int action, int mods) {
            auto* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
            if (r) r->onMouseButton(button, action, mods);
        });

    glfwSetCursorPosCallback(window.raw(),
        [](GLFWwindow* w, double xpos, double ypos) {
            auto* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
            if (r) r->onMouseMove(xpos, ypos);
        });

    glfwSetScrollCallback(window.raw(),
        [](GLFWwindow* w, double xoff, double yoff) {
            auto* r = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
            if (r) r->onMouseScroll(xoff, yoff);
        });
    glEnable(GL_DEPTH_TEST);

    std::cout << "Starting the loop." << std::endl;

    while (!window.shouldClose())
    {
        processInput(window.raw());

        // Rendering
        glClearColor(0.f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.draw();

        // Final steps
        window.swapAndPoll();
    }
    return 0;
}
