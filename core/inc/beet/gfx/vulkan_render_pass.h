#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {

class VulkanRenderPass {
   public:
    VulkanRenderPass(Renderer& renderer);
    ~VulkanRenderPass();

    void sync();

    VkSemaphore get_present_semaphore() { return m_presentSemaphore; }
    VkSemaphore get_render_semaphore() { return m_renderSemaphore; }
    VkFence get_render_fence() { return m_renderFence; }

    VkRenderPassBeginInfo create_begin_info();
    VkRenderPass get_render_pass() { return m_renderPass; }

   private:
    void init_default_renderpass();
    void init_framebuffers();
    void init_sync_structures();

   private:
    VkRenderPass m_renderPass{};
    std::vector<VkFramebuffer> m_framebuffers;

    VkSemaphore m_presentSemaphore;
    VkSemaphore m_renderSemaphore;
    VkFence m_renderFence;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx