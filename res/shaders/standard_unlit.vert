#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vUV;

layout (set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} cameraData;

layout (push_constant) uniform constants {
    vec4 data;
    mat4 render_matrix;
} PushConstants;

layout (location = 0) out StageLayout {
    vec3 color;
    vec3 normal;
    vec2 uv;
} stageLayout;

void main()
{
    mat4 transformMatrix = (cameraData.viewproj * PushConstants.render_matrix);
    gl_Position = transformMatrix * vec4(vPosition, 1.0f);
    stageLayout.color = vColor;
    stageLayout.uv = vUV;
    stageLayout.normal = vNormal;
}

