#version 330 core

uniform vec3 uColor;
uniform bool uSelected;

out vec4 FragColor;

void main()
{
    vec3 color = uColor;
    if (uSelected)
        color = mix(color, vec3(1.0, 0.45, 0.1), 0.5) + vec3(0.35);

    FragColor = vec4(color, 1.0);
}