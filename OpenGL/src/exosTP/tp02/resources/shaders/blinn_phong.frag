#version 330 core
smooth in vec3 vFragPos;
smooth in vec3 vNormal;
in vec2 vUV;

out vec4 FragColor;

uniform vec3 uLightDir;
uniform vec3 uCameraPos;
uniform vec3 uA, uD, uS;

uniform float ka = 0.2;
uniform float kd = 0.8;
uniform float ks = 0.5;

uniform float shininess = 32.0;


void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vFragPos);
    vec3 L = normalize(uLightDir);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    
    vec3 Ia = uA * ka;
    vec3 Id = uD * kd * NdotL;
    vec3 Is = (NdotL > 0.0) ? ks * uS * pow(NdotH, shininess) : vec3(0.0);

    vec3 result = Ia + Id + Is;

    FragColor = vec4(result, 1.0);
}

