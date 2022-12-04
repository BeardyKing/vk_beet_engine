#include <beet/assert.h>
#include <beet/gfx/vulkan_buffer.h>
#include <beet/gfx/vulkan_mesh.h>
#include <beet/renderer.h>
#define VMA_IMPLEMENTATION
#include <beet/log.h>
#include "vk_mem_alloc.h"

namespace beet::gfx {
VulkanBuffer::VulkanBuffer(Renderer& renderer) : m_renderer(renderer) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_renderer.get_physical_device();
    allocatorInfo.device = m_renderer.get_device();
    allocatorInfo.instance = m_renderer.get_instance();
    vmaCreateAllocator(&allocatorInfo, &m_allocator);
}

void VulkanBuffer::upload_mesh_immediate(Mesh& mesh) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = mesh.vertices.size() * sizeof(Vertex);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    auto result = vmaCreateBuffer(m_allocator, &bufferInfo, &vmaAllocInfo, &mesh.vertexBuffer.buffer,
                                  &mesh.vertexBuffer.allocation, nullptr);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "failed to create mesh buffer");

    void* data;
    vmaMapMemory(m_allocator, mesh.vertexBuffer.allocation, &data);
    memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(m_allocator, mesh.vertexBuffer.allocation);
}

void VulkanBuffer::destroy_mesh(Mesh& mesh) {
    //  TODO:FIXME This destruction should probably be managed by the resource manager or the mesh itself depending on
    //              the lifetime of the mesh as it could have multiple references.
    vmaDestroyBuffer(m_allocator, mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation);
}

VulkanBuffer::~VulkanBuffer() {
    vmaDestroyAllocator(m_allocator);
    log::debug("VulkanBuffer destroyed");
}

}  // namespace beet::gfx
