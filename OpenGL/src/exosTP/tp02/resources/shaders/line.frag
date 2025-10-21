#version 330 core

in vec3 gViewNormal;
out vec4 FragColor;

void main() {
	FragColor = vec4(gViewNormal, 1.0);
}
