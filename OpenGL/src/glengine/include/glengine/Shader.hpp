#pragma once
#include <string>
#include <vector>



unsigned handleShader(const std::string& filename, int layer, int shader_type);
unsigned createProgram(unsigned shaders);

unsigned createProgram(const std::vector<unsigned>& shaders);
