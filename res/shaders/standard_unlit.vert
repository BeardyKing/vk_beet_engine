#version 450

//===GLOBAL===//
layout (set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} cameraData;

//===LOCAL===//
layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec3 v_Color;
layout (location = 3) in vec2 v_UV;

layout (push_constant) uniform constants {
    vec4 data;
    mat4 render_matrix;
} PushConstants;

//===STAGE OUT===//
layout (location = 0) out StageLayout {
    vec3 color;
    vec3 normal;
    vec2 uv;
} stageLayout;

//===CODE===//
void main()
{
    mat4 transformMatrix = (cameraData.viewproj * PushConstants.render_matrix);
    gl_Position = transformMatrix * vec4(v_Position, 1.0f);
    stageLayout.color = v_Color;
    stageLayout.uv = v_UV;
    stageLayout.normal = v_Normal;
}

