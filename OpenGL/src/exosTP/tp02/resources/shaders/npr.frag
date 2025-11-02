#version 330 core
smooth in vec3 vFragPos;
smooth in vec3 vNormal;
in vec2 vUV;

in vec3 vNormal_view;
in vec3 vPos_view;

out vec4 FragColor;

uniform vec3 uLightDir;
uniform vec3 uCameraPos;
uniform vec3 uA, uD, uS;

uniform float ka = 0.6;
uniform float kd = 0.8;
uniform float ks = 0.9;

uniform float shininess = 32.0;

uniform float silBias = 0.2;
uniform float creaseBias = 0.12;

uniform vec4 thr = {60./255., 100./255., 150./255., 220./255.};
uniform vec3 dotColor = vec3(0.05);

vec3 nearest_color(vec3 original)
{
    float l = length(original);
    float s0 = step(thr.x, l);
    float s1 = step(thr.y, l);
    float s2 = step(thr.z, l);
    float s3 = step(thr.w, l);

    float top = 1.0;

    return vec3(
          thr.x * (1.0 - s0)
        + thr.y * (s0 - s1)
        + thr.z * (s1 - s2)
        + thr.w * (s2 - s3)
        + top   * s3
    );
}

vec3 toonize(vec3 original)
{
    float L = length(original);

    vec3 baseColor = nearest_color(original);

    // 1 if BELOW second threshold, else 0
    float below2 = 1.0 - step(thr.y, L);

    // binary 4× checker mask: 1 on every 4th pixel, else 0
    int ix = int(gl_FragCoord.x);
    int iy = int(gl_FragCoord.y);
    float dotMask = float((ix % 3) == 0 && (iy % 3 == 0));

    // apply dots only below threshold
    vec3 result = mix(baseColor, dotColor, below2 * dotMask);
    return result;
}

void main()
{
    // Phong Computation

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vFragPos);
    vec3 L = normalize(uLightDir);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    
    vec3 Ia = uA * ka;
    vec3 Id = uD * kd * NdotL;
    vec3 Is = ks * uS * pow(NdotH, shininess) * vec3(step(0.0001, NdotL));

    // Toon lighting

    vec3 toon = toonize(Ia + Id + Is);

    // Sharp edges

    vec3 Nv = normalize(vNormal_view);
    vec3 Vv = normalize(vPos_view);

    float sil = smoothstep(1.0 - silBias, 1.0, 1.0 - abs(dot(N, V)));

    float nGrad = length(dFdx(Nv)) + length(dFdy(Nv));
    float zGrad = fwidth(vPos_view.z);        // linear depth in view space
    float crease = smoothstep(creaseBias, creaseBias*2.0, nGrad + zGrad);

    float edge = clamp(max(sil, crease), 0.0, 1.0);

    vec3 outline = vec3(0.0);

    FragColor = vec4(mix(toon, outline, edge), 1.0);
}

