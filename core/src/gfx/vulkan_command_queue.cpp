#include <beet/gfx/vulkan_command_queue.h>

#include <beet/gfx/vulkan_initializers.h>

#include <beet/assert.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanCommandQueue::VulkanCommandQueue(Renderer& renderer) : m_renderer(renderer) {}

void VulkanCommandQueue::add_command(VkCommandBuffer cmd) {
    m_commandQueue.emplace_back(cmd);
}

void VulkanCommandQueue::submit() {
    auto presentSemaphore = m_renderer.get_present_semaphore();
    auto renderSemaphore = m_renderer.get_render_semaphore();
    auto renderFence = m_renderer.get_render_fence();
    auto graphicsQueue = m_renderer.get_graphics_queue();

    for (auto& cmd : m_commandQueue) {
        VkSubmitInfo submit = gfx::init::submit_info(&cmd);

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask = &waitStage;
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &presentSemaphore;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &renderSemaphore;

        auto result = vkQueueSubmit(graphicsQueue, 1, &submit, renderFence);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to submit to graphics queue");
    }

    m_commandQueue.clear();
}
VulkanCommandQueue::~VulkanCommandQueue() {
    log::debug("VulkanCommandQueue destroyed");
}

}  // namespace beet::gfx