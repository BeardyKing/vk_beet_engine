#pragma once
#include <vector>

#include <vulkan/vulkan.h>
namespace beet {
class Engine;
}  // namespace beet

namespace beet::gfx {
class VulkanDevice {
   public:
    VulkanDevice(Engine& engine);
    ~VulkanDevice();

    VkDevice get_device() { return m_device; }
    VkPhysicalDevice get_physical_device() { return m_chosenGPU; }
    VkSurfaceKHR get_surface() { return m_surface; }

   private:
    void init_vulkan();

   private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_chosenGPU;
    VkDevice m_device;
    VkSurfaceKHR m_surface;

   private:
    Engine& m_engine;
};
}  // namespace beet::gfx
