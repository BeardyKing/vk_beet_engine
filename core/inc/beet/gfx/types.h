#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace beet {
namespace gfx {

enum class ShaderType { Vertex, Fragment, LAST };
constexpr unsigned int FRAME_OVERLAP_COUNT = 2;

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct AllocatedImage {
    VkImage image;
    VmaAllocation allocation;
};

}  // namespace gfx
}  // namespace beet
