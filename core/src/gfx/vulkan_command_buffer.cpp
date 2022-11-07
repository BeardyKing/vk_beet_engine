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

    log::debug("VulkanCommandBuffer destroyed");
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
void VulkanCommandBuffer::reset() {
    auto result = vkResetCommandBuffer(m_mainCommandBuffer, 0);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to reset command buffer");
}

void VulkanCommandBuffer::begin_recording() {
    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;
    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    auto result = vkBeginCommandBuffer(m_mainCommandBuffer, &cmdBeginInfo);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to begin command buffer recording");
}

void VulkanCommandBuffer::end_recording() {
    auto result = vkEndCommandBuffer(m_mainCommandBuffer);
}

}  // namespace beet::gfx
