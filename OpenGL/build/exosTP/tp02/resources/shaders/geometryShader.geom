# version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

noperspective out vec3 vBary;

void main() {
    for (int i = 0; i < 3; ++i) {
        vBary = vec3(0, 0, 0); vBary[i] = 1;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
