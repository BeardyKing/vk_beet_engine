#include <beet/gfx/vulkan_command_buffer.h>
#include <beet/gfx/vulkan_initializers.h>

#include <beet/assert.h>
#include <beet/renderer.h>

namespace beet::gfx {
VulkanCommandBuffer::VulkanCommandBuffer(Renderer& renderer) : m_renderer(renderer) {
    init_commands();
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    auto device = m_renderer.get_device();

    vkDestroyCommandPool(device, m_commandPool, nullptr);
}

void VulkanCommandBuffer::init_commands() {
    auto device = m_renderer.get_device();
    auto queueFamily = m_renderer.get_queue_family();

    {
        VkCommandPoolCreateInfo commandPoolInfo =
            init::command_pool_create_info(queueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        auto result = vkCreateCommandPool(device, &commandPoolInfo, nullptr, &m_commandPool);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to create command pool");
    }
    {
        VkCommandBufferAllocateInfo cmdAllocInfo = init::command_buffer_allocate_info(m_commandPool, 1);

        auto result = vkAllocateCommandBuffers(device, &cmdAllocInfo, &m_mainCommandBuffer);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to allocate command buffers");
    }
}

}  // namespace beet::gfx
