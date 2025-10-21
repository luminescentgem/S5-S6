#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in   vec3 vViewPos[3];
in   vec3 vViewNormal[3]; 
in   vec2 vUV[3];

out vec3 gViewNormal[3];

uniform mat4 proj;
uniform float normalLength = 0.2;

void main () {
    vec3 center = (vViewPos[0] + vViewPos[1] + vViewPos[2]) / 3.0;
    vec3 dir    = normalize(vViewNormal[0] + vViewNormal[1] + vViewNormal[2]);
    
    gl_Position = proj * vec4(center, 1.0);
    EmitVertex();
    gl_Position = proj * vec4(center + dir * normalLength, 1.0);
    EmitVertex();

    EndPrimitive();
    gViewNormal = vViewNormal;
}