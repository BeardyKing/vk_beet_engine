#pragma once

#include <beet/gfx/vulkan_mesh.h>
#include <beet/gfx/vulkan_texture.h>

#include <beet/asset_loader.h>
#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/types.h>

#include <optional>

namespace beet {
struct DynamicViewport;
namespace gfx {
struct VulkanDevice;
struct VulkanBuffer;
struct VulkanDescriptors;
struct VulkanSwapchain;
struct VulkanCommandBuffer;
struct VulkanRenderPass;
struct VulkanCommandQueue;
struct Mesh;
struct Texture;
class VulkanPipeline;
}  // namespace gfx
}  // namespace beet

namespace beet {

class Renderer : public Subsystem {
   public:
    explicit Renderer();
    ~Renderer();

    static std::optional<std::reference_wrapper<Renderer>> get_renderer();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void recreate_swap_chain();
    void wait_idle();
    void render_sync();

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
    static size_t pad_uniform_buffer_size(size_t originalSize, const std::string& bufferName, bool verbose = false);
    // TODO: REPLACE WITH SAMPLER IN RES MANAGER
    VkSampler get_linear_sampler() { return m_linearSampler; };

   private:
    void update_camera_descriptor();
    void update_scene_descriptor();
    void set_clear_info(VkRenderPassBeginInfo& info);
    DynamicViewport update_viewport_scissor();

   private:
    inline static std::optional<std::reference_wrapper<Renderer>> s_renderer = std::nullopt;

   private:
    std::shared_ptr<gfx::VulkanDevice> m_device;
    std::shared_ptr<gfx::VulkanBuffer> m_buffer;
    std::shared_ptr<gfx::VulkanDescriptors> m_descriptors;
    std::shared_ptr<gfx::VulkanSwapchain> m_swapchain;
    std::shared_ptr<gfx::VulkanCommandBuffer> m_commandBuffer;
    std::shared_ptr<gfx::VulkanRenderPass> m_renderPass;
    std::shared_ptr<gfx::VulkanCommandQueue> m_commandQueue;

    gfx::GPUSceneData m_sceneData{
        vec4{0, -1, 0, 0}, vec4{1.0f, 0.0f, 1.0f, 1.0f}, vec4{0}, vec4{0}, vec4{0},
    };

    VkSampler m_linearSampler = {};
};

}  // namespace beet
