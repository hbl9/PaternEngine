#version 330 core

in vec3 vWorldPos; 

uniform float uTime;
uniform bool uSelected;

out vec4 FragColor;

void main()
{
    float r = 0.5 + 0.5 * sin(vWorldPos.x * 3.0 + uTime * 2.0);
    float g = 0.5 + 0.5 * sin(vWorldPos.y * 3.0 + uTime * 2.0 + 2.094); // +120°
    float b = 0.5 + 0.5 * sin(vWorldPos.z * 3.0 + uTime * 2.0 + 4.188); // +240°
    vec3 color = vec3(r, g, b);

    if (uSelected)
        color = mix(color, vec3(1.0, 0.45, 0.1), 0.5) + vec3(0.35);

    FragColor = vec4(color, 1.0);
}