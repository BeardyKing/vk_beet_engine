#version 450

//===GLOBAL===//
// layout must match that of `GPUSceneData` in beet/gfx/types.h
layout (set = 0, binding = 1) uniform SceneData {
    vec4 sunlightDirection;  // .xyz direction // .w power
    vec4 sunlightColor;      // .xyzw unused
    vec4 fogColor;           // .xyz color     // .w exponent
    vec4 fogDistances;       // .x min         // .y max       // .zw unused.
    //64
    vec4 ambientColor;       // .xyz color     // .w unused
    vec4 unused1;            // .xyzw unused
    vec4 unused2;            // .xyzw unused
    vec4 unused3;            // .xyzw unused
    //128
} sceneData;

//===LOCAL===//
layout (set = 1, binding = 0) uniform sampler2D u_albedo;
layout (set = 1, binding = 1) uniform sampler2D u_normal;
layout (set = 1, binding = 2) uniform sampler2D u_metallic;
layout (set = 1, binding = 3) uniform sampler2D u_roughness;
layout (set = 1, binding = 4) uniform sampler2D u_occlusion;

//===STAGE IN===//
layout (location = 0) in StageLayout {
    vec3 color;
    vec3 normal;
    vec2 uv;
} stageLayout;

//===OUT===//
layout (location = 0) out vec4 outFragColor;

//===CODE===//
void main()
{
    vec2 uv = stageLayout.uv;
    vec4 outcol = vec4(texture(u_albedo, uv).xyz, 1.0f);
    outcol = mix(outcol, vec4(texture(u_normal, uv).xyz, 1.0f), 0.5f);
    outcol = mix(outcol, vec4(texture(u_metallic, uv).xyz, 1.0f), 0.5f);
    outcol = mix(outcol, vec4(texture(u_roughness, uv).xyz, 1.0f), 0.5f);
    outcol = mix(outcol, vec4(texture(u_occlusion, uv).xyz, 1.0f), 0.5f);
    outFragColor = mix(outcol, sceneData.sunlightColor, 0.5f);
}
