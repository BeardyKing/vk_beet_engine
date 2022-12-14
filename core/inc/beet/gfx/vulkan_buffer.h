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
struct Texture;
}  // namespace beet::gfx

namespace beet::gfx {
class VulkanBuffer {
   public:
    VulkanBuffer(Renderer& renderer);
    ~VulkanBuffer();

    void upload_texture(Texture& texture);
    void destroy_texture(Texture& texture);

    void upload_mesh(Mesh& mesh);
    void destroy_mesh(Mesh& mesh);

    VmaAllocator get_allocator() { return m_allocator; }
    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    void destroy_buffer(AllocatedBuffer allocBuffer);
    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

   private:
    void init_immediate_commands();

   private:
    VmaAllocator m_allocator;
    UploadContext m_uploadContext;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
