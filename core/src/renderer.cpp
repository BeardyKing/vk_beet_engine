#include <beet/gfx/vulkan_device.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    m_device = std::make_shared<gfx::VulkanDevice>(m_engine);
    m_swapchain = std::make_shared<gfx::VulkanSwapchain>(*this);
    m_commandBuffer = std::make_shared<gfx::VulkanCommandBuffer>(*this);
    m_renderPass = std::make_shared<gfx::VulkanRenderPass>(*this);
}

void Renderer::on_awake() {}
void Renderer::on_update(double deltaTime) {}
void Renderer::on_late_update() {}
void Renderer::on_destroy() {}

Renderer::~Renderer() {
    log::debug("Renderer destroyed");
}

}  // namespace beet
