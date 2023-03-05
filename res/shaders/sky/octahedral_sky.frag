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

layout (set = 0, binding = 2) uniform sampler2D u_skyOct;

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
    outFragColor = vec4(texture(u_skyOct, uv).xyz, 1.0f);
}