#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace beet {
namespace gfx {

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct AllocatedImage {
    VkImage image;
    VmaAllocation allocation;
};

enum class ShaderType { Vertex, Fragment, LAST };

}  // namespace gfx
}  // namespace beet
