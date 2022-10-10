#pragma once

#define DEVICE_VERBOSE_LOGGING BEET_FALSE

#include <vulkan/vulkan.h>

#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/vertex.h>

#include <optional>
#include <string>

namespace beet {
class Engine;
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
}  // namespace beet

const std::vector<const char*> BEET_VK_VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> BEET_VK_DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const int MAX_FRAMES_IN_FLIGHT = 2;

namespace beet {
class Device : public Subsystem {
   public:
    Device(Engine& engine);
    ~Device();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void window_resized() { m_framebufferResized = true; }
    void recreate_swap_chain();

   private:
    void create_instance();
    void setup_debug_messenger();
    void create_surface();
    void pick_physical_device();
    void create_logical_device();
    void create_swap_chain();
    void create_image_views();
    void create_render_pass();
    void create_descriptor_set();
    void create_graphics_pipeline();
    void create_framebuffers();
    void create_command_pool();
    void create_vertex_buffer();
    void create_index_buffer();
    void create_uniform_buffers();
    void create_command_buffer();
    void create_sync_objects();

    void draw(double deltaTime);

   private:
    std::vector<const char*> get_required_extensions();

    bool check_validation_layer_support();
    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInformation);
    bool is_device_suitable(VkPhysicalDevice device);
    bool check_device_extension_support(VkPhysicalDevice device);
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkShaderModule create_shader_module(const std::vector<char>& code);
    void record_command_buffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void cleanup_swap_chain();
    uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void create_buffer(VkDeviceSize size,
                       VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags properties,
                       VkBuffer& buffer,
                       VkDeviceMemory& bufferMemory);

    void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void update_uniform_buffer(uint32_t currentFrame, double deltaTime);
   private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkSurfaceKHR m_surface;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    VkRenderPass m_renderPass;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    VkCommandPool m_commandPool;

    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;

    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;

    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    uint32_t m_currentFrame = 0;

    bool m_framebufferResized{false};

    const std::vector<uint16_t> m_indices = {0, 1, 2, 2, 3, 0};
    const std::vector<Vertex> m_vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

   private:
    Engine& m_engine;
};
}  // namespace beet

namespace beet {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool is_complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

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
