#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
using namespace glm;

namespace beet::gfx {

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct AllocatedImage {
    VkImage image;
    VmaAllocation allocation;
};

//===CPU->GPU CONTEXT===//
struct UploadContext {
    VkFence uploadFence;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
};

//===GLOBAL DESCRIPTOR===//
struct GPUCameraData {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
};

struct GPUSceneData {
    vec4 sunlightDirection;  // .xyz direction // .w power
    vec4 sunlightColor;      // .xyzw unused
    vec4 fogColor;           // .xyz color     // .w exponent
    vec4 fogDistances;       // .x min         // .y max       // .zw unused.
                             //
    vec4 ambientColor;       // .xyz color     // .w unused
    vec4 unused1;            // .xyzw unused
    vec4 unused2;            // .xyzw unused
    vec4 unused3;            // .xyzw unused
};

//===DOUBLE BUFFER CONTENTS===//
struct FrameData {
    VkSemaphore presentSemaphore;
    VkSemaphore renderSemaphore;
    VkFence renderFence;

    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;

    AllocatedBuffer cameraBuffer;
    VkDescriptorSet globalDescriptor;
};

constexpr unsigned int FRAME_OVERLAP_COUNT = 2;

enum class ShaderType { Vertex, Fragment, LAST };
enum class PipelineType { Invalid, Lit, LAST };
}  // namespace beet::gfx
