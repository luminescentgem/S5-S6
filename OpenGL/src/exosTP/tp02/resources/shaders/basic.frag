#version 330 core

smooth in   vec3 vNormal;

out vec4 fragColor;

void main()
{
    vec3 N = normalize(vNormal);
    fragColor = vec4(N * 0.5 + 0.6, 1.0); // solid orange
}

