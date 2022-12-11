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

//===GLOBAL DESCRIPTOR===//
struct GPUCameraData{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
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
}  // namespace beet::gfx
