#version 450

layout (location = 0) in StageLayout {
    vec3 color;
    vec3 normal;
    vec2 uv;
} stageLayout;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec4 tmpColor = vec4(stageLayout.uv, 0.0f, 1.0f);
    outFragColor = vec4(tmpColor);
}
