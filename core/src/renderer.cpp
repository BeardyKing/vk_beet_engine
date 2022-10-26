#define BEET_VULKAN_BACKEND BEET_TRUE

#ifdef BEET_VULKAN_BACKEND
#include <beet/gfx/vulkan_rhi/vulkan_device.h>
#endif

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
#ifdef BEET_VULKAN_BACKEND
    m_device = std::make_shared<gfx::VulkanDevice>(m_engine);
    m_device->init();
#endif
}

void Renderer::on_awake() {}
void Renderer::on_update(double deltaTime) {}
void Renderer::on_late_update() {}

void Renderer::on_destroy() {
    log::debug("Renderer destroyed");
}

Renderer::~Renderer() {}

}  // namespace beet
