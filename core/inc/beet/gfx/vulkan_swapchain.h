#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {
class VulkanSwapchain {
   public:
    VulkanSwapchain(Renderer& renderer);
    ~VulkanSwapchain();

   private:
    void init_swapchain();

   private:
    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchainImageFormat;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
