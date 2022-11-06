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

    VkFormat get_swapchain_image_format() { return m_swapchainImageFormat; }
    std::vector<VkImage> get_swapchain_images() { return m_swapchainImages; }
    std::vector<VkImageView> get_swapchain_image_views() { return m_swapchainImageViews; }

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
