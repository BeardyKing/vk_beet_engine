#pragma once

#include <beet/gfx/vulkan_buffer.h>
#include <beet/gfx/vulkan_command_buffer.h>
#include <beet/gfx/vulkan_command_queue.h>
#include <beet/gfx/vulkan_descriptors.h>
#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_mesh.h>
#include <beet/gfx/vulkan_pipeline.h>
#include <beet/gfx/vulkan_render_pass.h>
#include <beet/gfx/vulkan_shader_modules.h>
#include <beet/gfx/vulkan_swapchain.h>
#include <beet/gfx/vulkan_texture.h>

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

    void recreate_swap_chain();

    VkDevice get_device() { return m_device->get_device(); }
    VkPhysicalDevice get_physical_device() { return m_device->get_physical_device(); }
    VkSurfaceKHR get_surface() { return m_device->get_surface(); }
    VkQueue get_queue() { return m_device->get_queue(); }
    uint32_t get_queue_family() { return m_device->get_queue_family(); }
    VkQueue get_graphics_queue() { return m_device->get_queue(); }
    VkInstance get_instance() { return m_device->get_instance(); }
    void wait_idle() { m_device->wait_idle(); }

    VkSwapchainKHR get_swapchain() { return m_swapchain->get_swapchain(); }
    VkFormat get_swapchain_image_format() { return m_swapchain->get_swapchain_image_format(); }
    std::vector<VkImage> get_swapchain_images() { return m_swapchain->get_swapchain_images(); }
    std::vector<VkImageView> get_swapchain_image_views() { return m_swapchain->get_swapchain_image_views(); }
    VkImageView get_depth_image_view() { return m_swapchain->get_depth_image_view(); }
    uint32_t get_swapchain_index() { return m_swapchain->get_swapchain_index(); }
    VkFormat get_depth_format() { return m_swapchain->get_depth_format(); }

    void render_sync() { m_renderPass->sync(); }
    VkRenderPass get_render_pass() { return m_renderPass->get_render_pass(); }

    // TODO: VULKAN BUFFER FUNCTIONS SHOULD PROBABLY BE STATIC.
    //       UNLESS WE PASS THE BUFFER OBJECT AROUND INSTEAD.
    VmaAllocator get_allocator() { return m_buffer->get_allocator(); }
    gfx::AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
        return m_buffer->create_buffer(allocSize, usage, memoryUsage);
    }
    void destroy_buffer(gfx::AllocatedBuffer allocBuffer) { m_buffer->destroy_buffer(allocBuffer); }

    std::array<gfx::FrameData, gfx::FRAME_OVERLAP_COUNT>& get_frame_data() { return m_commandBuffer->get_frames(); }
    VkSemaphore& get_present_semaphore() { return m_commandBuffer->get_present_semaphore(); }
    VkSemaphore& get_render_semaphore() { return m_commandBuffer->get_render_semaphore(); }
    VkFence& get_render_fence() { return m_commandBuffer->get_render_fence(); }
    VkDescriptorSet& get_global_descriptor() { return m_commandBuffer->get_global_descriptor(); }

    VkDescriptorSetLayout get_global_descriptor_set() { return m_descriptors->get_global_descriptor_set(); }

   private:
    Engine& m_engine;

   private:
    std::shared_ptr<gfx::VulkanDevice> m_device;
    std::shared_ptr<gfx::VulkanBuffer> m_buffer;
    std::shared_ptr<gfx::VulkanDescriptors> m_descriptors;
    std::shared_ptr<gfx::VulkanSwapchain> m_swapchain;
    std::shared_ptr<gfx::VulkanCommandBuffer> m_commandBuffer;
    std::shared_ptr<gfx::VulkanRenderPass> m_renderPass;
    std::shared_ptr<gfx::VulkanCommandQueue> m_commandQueue;
    std::shared_ptr<gfx::VulkanPipeline> m_pipelineMesh;

    gfx::Mesh m_loadedMesh;
    gfx::Texture m_loadedTexture;

   private:  // tmp
    float m_timePassed{0};
};

}  // namespace beet
