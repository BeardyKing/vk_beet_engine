#pragma once

#include <beet/types.h>
//#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {
struct Mesh;
}  // namespace beet::gfx

namespace beet::gfx {
class VulkanBuffer {
   public:
    VulkanBuffer(Renderer& renderer);
    ~VulkanBuffer();

    void upload_mesh_immediate(Mesh& mesh);
    void destroy_mesh(Mesh& mesh);

   private:
    VmaAllocator m_allocator;

   private:
    Renderer& m_renderer;
};

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

}  // namespace beet::gfx
