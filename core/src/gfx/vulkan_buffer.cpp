#include <beet/assert.h>
#include <beet/gfx/vulkan_buffer.h>
#include <beet/gfx/vulkan_initializers.h>
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

    init_immediate_commands();
}

void VulkanBuffer::upload_mesh(Mesh& mesh) {
    const size_t bufferSize = mesh.vertices.size() * sizeof(Vertex);
    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.pNext = nullptr;

    stagingBufferInfo.size = bufferSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    AllocatedBuffer stagingBuffer;
    {
        auto result = vmaCreateBuffer(m_allocator, &stagingBufferInfo, &vmaAllocInfo, &stagingBuffer.buffer,
                                      &stagingBuffer.allocation, nullptr);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "failed to create CPU mesh buffer");
    }

    void* data;
    vmaMapMemory(m_allocator, stagingBuffer.allocation, &data);
    memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(m_allocator, stagingBuffer.allocation);

    VkBufferCreateInfo vertexBufferInfo = {};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.pNext = nullptr;
    vertexBufferInfo.size = bufferSize;
    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    {
        auto result = vmaCreateBuffer(m_allocator, &vertexBufferInfo, &vmaAllocInfo, &mesh.vertexBuffer.buffer,
                                      &mesh.vertexBuffer.allocation, nullptr);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "failed to create GPU mesh buffer");
    }

    immediate_submit([=](VkCommandBuffer cmd) {
        VkBufferCopy copy;
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size = bufferSize;
        vkCmdCopyBuffer(cmd, stagingBuffer.buffer, mesh.vertexBuffer.buffer, 1, &copy);
    });

    vmaDestroyBuffer(m_allocator, stagingBuffer.buffer, stagingBuffer.allocation);
}

void VulkanBuffer::destroy_mesh(Mesh& mesh) {
    //  TODO:FIXME This destruction should probably be managed by the resource manager or the mesh itself depending on
    //              the lifetime of the mesh as it could have multiple references.
    vmaDestroyBuffer(m_allocator, mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation);
}

VulkanBuffer::~VulkanBuffer() {
    auto device = m_renderer.get_device();

    vmaDestroyAllocator(m_allocator);
    vkDestroyCommandPool(device, m_uploadContext.commandPool, nullptr);
    vkDestroyFence(device, m_uploadContext.uploadFence, nullptr);

    log::debug("VulkanBuffer destroyed");
}

AllocatedBuffer VulkanBuffer::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = allocSize;
    bufferInfo.usage = usage;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = memoryUsage;

    AllocatedBuffer newBuffer{};
    auto result =
        vmaCreateBuffer(m_allocator, &bufferInfo, &vmaAllocInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "failed to create AllocatedBuffer");

    return newBuffer;
}

void VulkanBuffer::destroy_buffer(AllocatedBuffer allocBuffer) {
    vmaDestroyBuffer(m_allocator, allocBuffer.buffer, allocBuffer.allocation);
}

void VulkanBuffer::init_immediate_commands() {
    auto device = m_renderer.get_device();
    auto graphicsQueue = m_renderer.get_graphics_queue();
    auto queueFamily = m_renderer.get_queue_family();

    VkCommandPoolCreateInfo uploadCommandPoolInfo = init::command_pool_create_info(queueFamily);
    {
        auto result = vkCreateCommandPool(device, &uploadCommandPoolInfo, nullptr, &m_uploadContext.commandPool);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to allocate command pool for upload context");
    }
    {
        VkCommandBufferAllocateInfo cmdAllocInfo = init::command_buffer_allocate_info(m_uploadContext.commandPool, 1);
        VkCommandBuffer cmd;
        auto result = vkAllocateCommandBuffers(device, &cmdAllocInfo, &m_uploadContext.commandBuffer);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS,
                            "Error: Vulkan failed to allocate command buffers for upload context");
    }

    VkFenceCreateInfo uploadFenceCreateInfo = init::fence_create_info();

    auto result = (vkCreateFence(device, &uploadFenceCreateInfo, nullptr, &m_uploadContext.uploadFence));
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to create fence for upload context");
}

void VulkanBuffer::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {
    auto device = m_renderer.get_device();
    auto graphicsQueue = m_renderer.get_graphics_queue();

    VkCommandBuffer cmd = m_uploadContext.commandBuffer;
    VkCommandBufferBeginInfo cmdBeginInfo =
        init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    {
        auto result = vkBeginCommandBuffer(cmd, &cmdBeginInfo);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS,
                            "Error: Vulkan failed to begin command buffer recording for upload context")
    }
    function(cmd);
    {
        auto result = vkEndCommandBuffer(cmd);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS,
                            "Error: Vulkan failed to finish command buffer recording for upload context");
    }
    VkSubmitInfo submit = init::submit_info(&cmd);
    {
        auto result = vkQueueSubmit(graphicsQueue, 1, &submit, m_uploadContext.uploadFence);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to upload command buffer for upload context");
    }

    vkWaitForFences(device, 1, &m_uploadContext.uploadFence, true, 9999999999);
    vkResetFences(device, 1, &m_uploadContext.uploadFence);
    vkResetCommandPool(device, m_uploadContext.commandPool, 0);
}

}  // namespace beet::gfx
