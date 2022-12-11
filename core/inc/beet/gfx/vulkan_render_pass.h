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

    VkRenderPassBeginInfo create_begin_info();
    VkRenderPass get_render_pass() { return m_renderPass; }
    void recreate();

   private:
    void init_default_renderpass();
    void init_framebuffers();
    void init_sync_structures();

    void cleanup();
   private:
    VkRenderPass m_renderPass{};
    std::vector<VkFramebuffer> m_framebuffers;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx