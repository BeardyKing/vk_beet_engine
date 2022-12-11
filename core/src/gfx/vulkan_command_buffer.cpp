#include <beet/gfx/vulkan_command_buffer.h>
#include <beet/gfx/vulkan_initializers.h>

#include <beet/assert.h>
#include <beet/renderer.h>

namespace beet::gfx {
VulkanCommandBuffer::VulkanCommandBuffer(Renderer& renderer) : m_renderer(renderer) {
    init_commands();
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    cleanup();

    for (auto& frame : m_frames) {
        m_renderer.destroy_buffer(frame.cameraBuffer);
    }

    log::debug("VulkanCommandBuffer destroyed");
}

void VulkanCommandBuffer::recreate() {
    cleanup();
    init_commands();
}

void VulkanCommandBuffer::cleanup() {
    auto device = m_renderer.get_device();

    for (auto& frame : m_frames) {
        vkDestroyCommandPool(device, frame.commandPool, nullptr);
    }

    for (auto& frame : m_frames) {
        vkDestroyFence(device, frame.renderFence, nullptr);
        vkDestroySemaphore(device, frame.renderSemaphore, nullptr);
        vkDestroySemaphore(device, frame.presentSemaphore, nullptr);
    }
}

void VulkanCommandBuffer::init_commands() {
    auto device = m_renderer.get_device();
    auto queueFamily = m_renderer.get_queue_family();

    VkCommandPoolCreateInfo commandPoolInfo =
        init::command_pool_create_info(queueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& frame : m_frames) {
        {
            auto result = vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frame.commandPool);
            BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to create command pool");
        }
        {
            VkCommandBufferAllocateInfo cmdAllocInfo = init::command_buffer_allocate_info(frame.commandPool, 1);
            auto result = vkAllocateCommandBuffers(device, &cmdAllocInfo, &frame.mainCommandBuffer);
            BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to allocate command buffers");
        }
    }
}
void VulkanCommandBuffer::reset() {
    auto result = vkResetCommandBuffer(get_main_command_buffer(), 0);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to reset command buffer");
}

void VulkanCommandBuffer::begin_recording() {
    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;
    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    auto result = vkBeginCommandBuffer(get_main_command_buffer(), &cmdBeginInfo);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to begin command buffer recording");
}

void VulkanCommandBuffer::end_recording() {
    auto result = vkEndCommandBuffer(get_main_command_buffer());
}

}  // namespace beet::gfx
