#pragma once

#include <beet/gfx/vulkan_command_buffer.h>
#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_render_pass.h>
#include <beet/gfx/vulkan_swapchain.h>

#include <beet/asset_loader.h>
#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/types.h>

namespace beet {
class Engine;
}  // namespace beet

namespace beet {

class Renderer : public Subsystem {
   public:
    explicit Renderer(Engine& engine);
    ~Renderer();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    Engine& get_engine() { return m_engine; }

    VkDevice get_device() { return m_device->get_device(); }
    VkPhysicalDevice get_physical_device() { return m_device->get_physical_device(); }
    VkSurfaceKHR get_surface() { return m_device->get_surface(); }
    VkQueue get_queue() { return m_device->get_queue(); }
    uint32_t get_queue_family() { return m_device->get_queue_family(); }
    VkQueue get_graphics_queue() { return m_device->get_queue(); }

    VkSwapchainKHR get_swapchain() { return m_swapchain->get_swapchain(); }
    VkFormat get_swapchain_image_format() { return m_swapchain->get_swapchain_image_format(); }
    std::vector<VkImage> get_swapchain_images() { return m_swapchain->get_swapchain_images(); }
    std::vector<VkImageView> get_swapchain_image_views() { return m_swapchain->get_swapchain_image_views(); }
    uint32_t get_swapchain_index() { return m_swapchain->get_swapchain_index(); }

    void render_sync() { m_renderPass->sync(); }
    VkSemaphore get_present_semaphore() { return m_renderPass->get_present_semaphore(); }
    VkSemaphore get_render_semaphore() { return m_renderPass->get_render_semaphore(); }
    VkFence get_render_fence() { return m_renderPass->get_render_fence(); }

   private:
    Engine& m_engine;

   private:
    std::shared_ptr<gfx::VulkanDevice> m_device;
    std::shared_ptr<gfx::VulkanSwapchain> m_swapchain;
    std::shared_ptr<gfx::VulkanCommandBuffer> m_commandBuffer;
    std::shared_ptr<gfx::VulkanRenderPass> m_renderPass;

   private:  // tmp
    float m_timePassed{0};
};

}  // namespace beet
