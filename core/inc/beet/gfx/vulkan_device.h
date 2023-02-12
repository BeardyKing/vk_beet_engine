#pragma once
#include <vector>

#include <vulkan/vulkan.h>
namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {
class VulkanDevice {
   public:
    VulkanDevice(Renderer& renderer);
    ~VulkanDevice();

    VkDevice get_device() { return m_device; }
    VkPhysicalDevice get_physical_device() { return m_chosenGPU; }
    VkSurfaceKHR get_surface() { return m_surface; }
    VkQueue get_graphics_queue() { return m_graphicsQueue; }
    uint32_t get_queue_family() { return m_graphicsQueueFamily; }
    VkPhysicalDeviceProperties get_gpu_properties() { return m_gpuProperties; };
    VkInstance get_instance() { return m_instance; }

    void wait_idle() { vkDeviceWaitIdle(m_device); }

   private:
    void init_vulkan();

   private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_chosenGPU;
    VkPhysicalDeviceProperties m_gpuProperties;
    VkDevice m_device;
    VkSurfaceKHR m_surface;

    VkQueue m_graphicsQueue;
    uint32_t m_graphicsQueueFamily;

   private:
    Renderer& m_renderer;
};
}  // namespace beet::gfx
