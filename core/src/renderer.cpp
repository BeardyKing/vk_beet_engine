#include <beet/gfx/vulkan_device.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    m_device = std::make_shared<gfx::VulkanDevice>(*this);
    m_swapchain = std::make_shared<gfx::VulkanSwapchain>(*this);
    m_commandBuffer = std::make_shared<gfx::VulkanCommandBuffer>(*this);
    m_renderPass = std::make_shared<gfx::VulkanRenderPass>(*this);
    m_commandQueue = std::make_shared<gfx::VulkanCommandQueue>(*this);
}

void Renderer::on_awake() {}

void Renderer::on_update(double deltaTime) {
    VkClearValue clearValue;
    clearValue.color = {{0.5f, 0.092, 0.167f, 1.0f}};

    render_sync();
    m_commandBuffer->reset();
    m_swapchain->acquire_next_image();

    auto info = m_renderPass->create_begin_info();
    info.clearValueCount = 1;
    info.pClearValues = &clearValue;

    auto cmd = m_commandBuffer->get_main_command_buffer();
    {
        m_commandBuffer->begin_recording();
        {
            vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
            // commands here
            vkCmdEndRenderPass(cmd);
        }
        m_commandBuffer->end_recording();
    }
    m_commandQueue->add_command(cmd);

    m_commandQueue->submit();
    m_swapchain->present();

    m_timePassed += (float)deltaTime;
}

void Renderer::on_late_update() {}
void Renderer::on_destroy() {}

Renderer::~Renderer() {
    log::debug("Renderer destroyed");
}

}  // namespace beet
