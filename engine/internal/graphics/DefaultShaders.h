#pragma once

namespace DefaultShaders
{
    const char* BASIC_VERT = R"(
#version 330 core
layout (location = 0) in vec3 vPos;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
out vec3 vWorldPos;
void main()
{
    vec4 world = uModel * vec4(vPos, 1.0);
    vWorldPos = world.xyz;
    gl_Position = uProjection * uView * uModel * vec4(vPos, 1.0);
})";

    const char* BASIC_FRAG = R"(
#version 330 core
in vec3 vWorldPos; 
uniform float uTime;
uniform bool uSelected;
out vec4 FragColor;
void main()
{
    float r = 0.5 + 0.5 * sin(vWorldPos.x * 3.0 + uTime * 2.0);
    float g = 0.5 + 0.5 * sin(vWorldPos.y * 3.0 + uTime * 2.0 + 2.094);
    float b = 0.5 + 0.5 * sin(vWorldPos.z * 3.0 + uTime * 2.0 + 4.188);
    vec3 color = vec3(r, g, b);
    if (uSelected) color = mix(color, vec3(1.0, 0.45, 0.1), 0.5) + vec3(0.35);
    FragColor = vec4(color, 1.0);
})";

    const char* PLANE_VERT = R"(
#version 330 core
layout (location = 0) in vec3 vPos;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
void main()
{
    gl_Position = uProjection * uView * uModel * vec4(vPos, 1.0);
})";

    const char* PLANE_FRAG = R"(
#version 330 core
uniform vec3 uColor;
uniform bool uSelected;
out vec4 FragColor;
void main()
{
    vec3 color = uColor;
    if (uSelected) color = mix(color, vec3(1.0, 0.45, 0.1), 0.5) + vec3(0.35);
    FragColor = vec4(color, 1.0);
})";
}