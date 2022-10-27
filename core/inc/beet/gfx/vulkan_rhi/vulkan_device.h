#pragma once
#include <beet/gfx/i_device.h>
#include <vector>

#include <vulkan/vulkan.h>
namespace beet {
class Engine;
}  // namespace beet

namespace beet::gfx {
class VulkanDevice : public IDevice {
   public:
    explicit VulkanDevice(Engine& engine);
    ~VulkanDevice();

    bool init() override;

   private:
    void init_vulkan();
    void init_swapchain();

   private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_chosenGPU;
    VkDevice m_device;
    VkSurfaceKHR m_surface;

    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchainImageFormat;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;

   private:
    Engine& m_engine;
};
}  // namespace beet::gfx
