#include <beet/gfx/vulkan_initializers.h>

namespace beet::gfx::init {

VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.queueFamilyIndex = queueFamilyIndex;
    info.flags = flags;
    return info;
}

VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool,
                                                         uint32_t count,
                                                         VkCommandBufferLevel level) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = level;
    return info;
}

VkSubmitInfo init::submit_info(VkCommandBuffer* cmd)
{
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pNext = nullptr;

    info.waitSemaphoreCount = 0;
    info.pWaitSemaphores = nullptr;
    info.pWaitDstStageMask = nullptr;
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd;
    info.signalSemaphoreCount = 0;
    info.pSignalSemaphores = nullptr;

    return info;
}

VkPresentInfoKHR init::present_info()
{
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pNext = nullptr;

    info.swapchainCount = 0;
    info.pSwapchains = nullptr;
    info.pWaitSemaphores = nullptr;
    info.waitSemaphoreCount = 0;
    info.pImageIndices = nullptr;

    return info;
}

VkRenderPassBeginInfo init::renderpass_begin_info(VkRenderPass renderPass,
                                                  VkExtent2D windowExtent,
                                                  VkFramebuffer framebuffer) {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;

    info.renderPass = renderPass;
    info.renderArea.offset.x = 0;
    info.renderArea.offset.y = 0;
    info.renderArea.extent = windowExtent;
    info.clearValueCount = 1;
    info.pClearValues = nullptr;
    info.framebuffer = framebuffer;

    return info;
}

}  // namespace beet::gfx::init