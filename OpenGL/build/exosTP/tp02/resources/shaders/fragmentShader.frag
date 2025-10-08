#version 330 core
noperspective in vec3 vBary;
out vec4 fragColor;
uniform float line_size;
float edgeDist = min(min(vBary.x, vBary.y), vBary.z);
// optional AA:
float w = fwidth(edgeDist);
float mask = 1.0 - smoothstep(line_size - w, line_size + w, edgeDist);
fragColor = mix(vec4(1.0), vec4(0.0), mask);
