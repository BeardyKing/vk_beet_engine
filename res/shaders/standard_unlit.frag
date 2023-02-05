#version 450

layout (set = 1, binding = 0) uniform sampler2D u_albedo;

layout (location = 0) in StageLayout {
    vec3 color;
    vec3 normal;
    vec2 uv;
} stageLayout;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec2 uv = stageLayout.uv;
    vec4 tmpColor = vec4(texture(u_albedo, uv).xyz, 1.0f);
    outFragColor = vec4(tmpColor);
}
