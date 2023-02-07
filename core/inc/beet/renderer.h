#pragma once

#include <optional>

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

#include <beet/component/material.h>

#include <beet/asset_loader.h>
#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/types.h>

namespace beet {
class Engine;
class Transform;
class Camera;
struct DynamicViewport;
}  // namespace beet

namespace beet {

class Renderer : public Subsystem {
   public:
    explicit Renderer(Engine& engine);
    ~Renderer();

    static std::optional<std::reference_wrapper<Renderer>> get_renderer();
    Engine& get_engine() { return m_engine; }

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void recreate_swap_chain();
    void wait_idle() { m_device->wait_idle(); }
    void render_sync() { m_renderPass->sync(); }

    std::shared_ptr<gfx::VulkanDevice> get_vulkan_device() { return m_device; };
    std::shared_ptr<gfx::VulkanBuffer> get_vulkan_buffer() { return m_buffer; };
    std::shared_ptr<gfx::VulkanDescriptors> get_vulkan_descriptor() { return m_descriptors; };
    std::shared_ptr<gfx::VulkanSwapchain> get_vulkan_swapchain() { return m_swapchain; };
    std::shared_ptr<gfx::VulkanCommandBuffer> get_vulkan_command_buffer() { return m_commandBuffer; };
    std::shared_ptr<gfx::VulkanRenderPass> get_vulkan_render_pass() { return m_renderPass; };
    std::shared_ptr<gfx::VulkanCommandQueue> get_vulkan_command_queue() { return m_commandQueue; };

    // TODO: VK UTIL FUNCTIONS MOVE TO UTIL CLASS
    void upload_texture(gfx::Texture& texture);
    void destroy_texture(gfx::Texture& texture);
    void create_image_view(gfx::Texture& texture);
    void destroy_image_view(gfx::Texture& texture);
    void upload_mesh(gfx::Mesh& mesh);
    void destroy_mesh(gfx::Mesh& mesh);
    std::shared_ptr<gfx::VulkanPipeline> generate_lit_pipeline();
    // TODO: REPLACE WITH SAMPLER IN RES MANAGER
    VkSampler get_linear_sampler() { return m_linearSampler; };

   private:
    void update_camera_descriptor();
    void set_clear_info(VkRenderPassBeginInfo& info);
    DynamicViewport update_viewport_scissor();

   private:
    Engine& m_engine;
    inline static std::optional<std::reference_wrapper<Renderer>> s_renderer = std::nullopt;

   private:
    std::shared_ptr<gfx::VulkanDevice> m_device;
    std::shared_ptr<gfx::VulkanBuffer> m_buffer;
    std::shared_ptr<gfx::VulkanDescriptors> m_descriptors;
    std::shared_ptr<gfx::VulkanSwapchain> m_swapchain;
    std::shared_ptr<gfx::VulkanCommandBuffer> m_commandBuffer;
    std::shared_ptr<gfx::VulkanRenderPass> m_renderPass;
    std::shared_ptr<gfx::VulkanCommandQueue> m_commandQueue;

    VkSampler m_linearSampler = {};
};

}  // namespace beet
