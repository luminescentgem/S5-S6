#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;

out vec3 vViewPos;
out vec3 vViewNormal;
out vec2 vUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 uMVP;

void main() {
	mat4 mv = view * model;
	mat3 mvn = transpose(inverse(mat3(mv)));
	
	vViewPos	= (mv * vec4(aPos, 1.0)).xyz;
	vViewNormal = normalize(mvn * aNormal);
	vUV			= aUV;
	gl_Position = uMVP * vec4(aPos, 1.0);
}
