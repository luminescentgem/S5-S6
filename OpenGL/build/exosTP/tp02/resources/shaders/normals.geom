#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

smooth in   vec3 vNormal[];
smooth in   vec2 vUV[];

smooth out  vec3 gNormal;
smooth out  vec2 gUV;

void main () {
    
}