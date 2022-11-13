#include <beet/gfx/vulkan_device.h>

#include <beet/assert.h>
#include <beet/asset_loader.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/types.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    m_device = std::make_shared<gfx::VulkanDevice>(*this);
    m_swapchain = std::make_shared<gfx::VulkanSwapchain>(*this);
    m_commandBuffer = std::make_shared<gfx::VulkanCommandBuffer>(*this);
    m_renderPass = std::make_shared<gfx::VulkanRenderPass>(*this);
    m_commandQueue = std::make_shared<gfx::VulkanCommandQueue>(*this);
    m_pipeline = std::make_shared<gfx::VulkanPipeline>(*this);
}

void Renderer::on_awake() {
    //  RESOURCES: TODO: REPLACE WITH RESOURCE MANAGER
    auto vertSrc = AssetLoader::read_file("../res/shaders/triangle_shader.vert.spv");
    auto fragSrc = AssetLoader::read_file("../res/shaders/triangle_shader.frag.spv");
    gfx::VulkanShaderModules triangleShader(*this);
    triangleShader.load(vertSrc, gfx::ShaderType::Vertex);
    triangleShader.load(fragSrc, gfx::ShaderType::Fragment);
    //  RESOURCES:

    m_pipeline->add_stages(triangleShader);
    m_pipeline->build();
}

void Renderer::on_update(double deltaTime) {
    VkClearValue clearValue;
    clearValue.color = {{0.5f, 0.092, 0.167f, 1.0f}};

    // TODO: re-create swapchain on resize / minimise.
    
    render_sync();
    m_commandBuffer->reset();
    m_swapchain->acquire_next_image();

    auto info = m_renderPass->create_begin_info();
    info.clearValueCount = 1;
    info.pClearValues = &clearValue;

    auto cmd = m_commandBuffer->get_main_command_buffer();
    {
        m_commandBuffer->begin_recording();
        {
            vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
            {
                // commands here
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get_pipeline());
                vkCmdDraw(cmd, 3, 1, 0, 0);
            }
            vkCmdEndRenderPass(cmd);
        }
        m_commandBuffer->end_recording();
    }
    m_commandQueue->add_command(cmd);

    m_commandQueue->submit();
    m_swapchain->present();

    m_timePassed += (float)deltaTime;
}

void Renderer::on_late_update() {}

void Renderer::on_destroy() {
    render_sync();
}

Renderer::~Renderer() {
    log::debug("Renderer destroyed");
}

}  // namespace beet
