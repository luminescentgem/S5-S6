#version 330 core

layout (location=0) in vec3 aPos;
layout (location=0) in vec3 aNormal;
layout (location=0) in vec2 aUV;

smooth          out vec3 vNormal;
smooth          out vec2 vUV;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vNormal = normalize(uNormalMatrix * aNormal);
    vUV = aUV;
}
