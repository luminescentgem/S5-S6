#include "glengine/Shader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

static std::string readTextFile(std::string path) {
    // Récupération du shader
    std::string shader_code;
    std::ifstream shader_file;

    shader_file.open(path.c_str());
    if (!shader_file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    // Lecture du fichier
    std::stringstream iss;
    iss << shader_file.rdbuf();
    shader_file.close();
    shader_code = iss.str();
    return shader_code;
}

unsigned handleShader(const std::string& filename, int /*layer*/, int shader_type)
{
    std::string shader_code = readTextFile(filename);
    const char* shaderSource = shader_code.c_str();

    // Compilation du shader
    unsigned shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    // Vérifier l'état de la compilation du shader
    int success = 0;
    char infoLog[1024] = {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::"
                  << (shader_type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}

unsigned createProgram(unsigned shader)
{
    unsigned shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, shader);

    glLinkProgram(shaderProgram);

    // Vérifier l'état de la compilation du programme
    int success = 0;
    char infoLog[1024] = {};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        throw std::runtime_error("Program link failed");
    }

    // Nettoie les shaders attachés
    glDeleteShader(shader);

    return shaderProgram;
}

unsigned createProgram(const std::vector<unsigned>& shaders)
{
    unsigned shaderProgram = glCreateProgram();

    for (unsigned shader : shaders)
        glAttachShader(shaderProgram, shader);

    glLinkProgram(shaderProgram);

    // Vérifier l'état de la compilation du programme
    int success = 0;
    char infoLog[1024] = {};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        throw std::runtime_error("Program link failed");
    }

    // Nettoie les shaders attachés
    for (unsigned shader : shaders)
        glDeleteShader(shader);

    return shaderProgram;
}

