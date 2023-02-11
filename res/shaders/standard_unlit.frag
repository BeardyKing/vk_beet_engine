#version 450

layout (set = 1, binding = 0) uniform sampler2D u_albedo;
layout (set = 1, binding = 1) uniform sampler2D u_normal;
layout (set = 1, binding = 2) uniform sampler2D u_metallic;
layout (set = 1, binding = 3) uniform sampler2D u_roughness;
layout (set = 1, binding = 4) uniform sampler2D u_occlusion;

layout (location = 0) out vec4 outFragColor;

layout (location = 0) in StageLayout {
    vec3 color;
    vec3 normal;
    vec2 uv;
} stageLayout;

void main()
{
    vec2 uv = stageLayout.uv;
    vec4 outcol = vec4(texture(u_albedo, uv).xyz, 1.0f);
    outcol = mix(outcol, vec4(texture(u_normal, uv).xyz, 1.0f), 0.5f);
    outcol = mix(outcol, vec4(texture(u_metallic, uv).xyz, 1.0f), 0.5f);
    outcol = mix(outcol, vec4(texture(u_roughness, uv).xyz, 1.0f), 0.5f);
    outcol = mix(outcol, vec4(texture(u_occlusion, uv).xyz, 1.0f), 0.5f);
    outFragColor = outcol;
}
