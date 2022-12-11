#pragma once

#include <beet/gfx/types.h>

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

    VkSwapchainKHR get_swapchain() { return m_swapchain; }
    VkFormat get_swapchain_image_format() { return m_swapchainImageFormat; }
    std::vector<VkImage> get_swapchain_images() { return m_swapchainImages; }
    std::vector<VkImageView> get_swapchain_image_views() { return m_swapchainImageViews; }
    VkImageView get_depth_image_view() { return m_depthImageView; }
    VkFormat get_depth_format() { return m_depthFormat; }

    VkResult acquire_next_image();
    uint32_t get_swapchain_index() { return m_swapchainIndex; }

    void present();

    void recreate();

   private:
    void init_swapchain();
    void cleanup();

   private:
    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchainImageFormat;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;

    VkImageView m_depthImageView;
    AllocatedImage m_depthImage;
    VkFormat m_depthFormat;

   private:
    uint32_t m_swapchainIndex{};

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
