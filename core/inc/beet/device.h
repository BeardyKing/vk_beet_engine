#pragma once

#define DEVICE_VERBOSE_LOGGING BEET_FALSE

#include <vulkan/vulkan.h>

#include <beet/log.h>
#include <beet/subsystem.h>

#include <optional>
#include <string>

namespace beet {
class Engine;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool is_complete() { return graphicsFamily.has_value(); }
};

namespace beet {
class Device : public Subsystem {
   public:
    Device(Engine& engine);
    ~Device();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

   private:
    void create_instance();
    void setup_debug_messenger();
    void pick_physical_device();
    void create_logical_device();

   private:
    std::vector<const char*> get_required_extensions();

    bool check_validation_layer_support();
    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInformation);
    bool is_device_suitable(VkPhysicalDevice device);
    static QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
   private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphicsQueue;

   private:
    Engine& m_engine;
};
}  // namespace beet

namespace beet {

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

VkResult create_debug_utils_messenger_EXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);
void destroy_debug_utils_messengerEXT(VkInstance instance,
                                      VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks* pAllocator);

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                     void* pUserData);

#ifdef BEET_DEBUG
constexpr bool ENABLED_VALIDATION_LAYERS = true;
#else
constexpr bool ENABLED_VALIDATION_LAYERS = false;
#endif
}  // namespace beet
