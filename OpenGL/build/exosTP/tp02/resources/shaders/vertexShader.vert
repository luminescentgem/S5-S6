#version 330 core

layout (location=0) in vec3 aPos;
layout (location=0) in vec3 aNormal;
layout (location=0) in vec3 aUV;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
}
