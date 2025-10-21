#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aUV;

smooth out vec3 vFragPos;
smooth out vec3 vNormal;
out vec2 vUV;
out vec3 vNormal_view;
out vec3 vPos_view;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat3 uNormalMatrix;

void main() {
    vFragPos = (uModel * vec4(aPos, 1.0)).xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vUV = aUV;

    vNormal_view = mat3(transpose(inverse(uView * uModel))) * aNormal;
    vPos_view = (uView * uModel * vec4(aPos, 1.0)).xyz;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
