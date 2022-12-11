#pragma once

#include <beet/gfx/types.h>
#include <beet/types.h>
// #define VMA_IMPLEMENTATION
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

    VmaAllocator get_allocator() { return m_allocator; }

    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    void destroy_buffer(AllocatedBuffer allocBuffer);

   private:
    VmaAllocator m_allocator;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
