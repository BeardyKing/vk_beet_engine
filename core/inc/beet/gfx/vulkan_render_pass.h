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

   private:
    void init_default_renderpass();
    void init_framebuffers();

   private:
    VkRenderPass m_renderPass{};
    std::vector<VkFramebuffer> m_framebuffers;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx