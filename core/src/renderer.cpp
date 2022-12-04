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

    m_pipeline_red = std::make_shared<gfx::VulkanPipeline>(*this);
    m_pipeline_col = std::make_shared<gfx::VulkanPipeline>(*this);
}

void Renderer::on_awake() {
    //  RESOURCES: TODO: REPLACE WITH RESOURCE MANAGER
    {
        //  RESOURCES:    RED TRIANGLE
        auto triRedVertSrc = AssetLoader::read_file("../res/shaders/triangle_shader.vert.spv");
        auto triRedFragSrc = AssetLoader::read_file("../res/shaders/triangle_shader.frag.spv");
        gfx::VulkanShaderModules redTriangleShader(*this);
        redTriangleShader.load(triRedVertSrc, gfx::ShaderType::Vertex);
        redTriangleShader.load(triRedFragSrc, gfx::ShaderType::Fragment);
        m_pipeline_red->add_stages(redTriangleShader);
        m_pipeline_red->build();
    }
    {
        // RESOURCES: COLOURED TRIANGLE
        auto triColVertSrc = AssetLoader::read_file("../res/shaders/color_triangle_shader.vert.spv");
        auto triColFragSrc = AssetLoader::read_file("../res/shaders/color_triangle_shader.frag.spv");
        gfx::VulkanShaderModules colTriangleShader(*this);
        colTriangleShader.load(triColVertSrc, gfx::ShaderType::Vertex);
        colTriangleShader.load(triColFragSrc, gfx::ShaderType::Fragment);
        m_pipeline_col->add_stages(colTriangleShader);
        m_pipeline_col->build();
    }
}

void Renderer::on_update(double deltaTime) {
    //  TMP: INPUT CODE
    const auto input = m_engine.get_window_module().lock()->get_input_manager();
    if (input->key_pressed(KeyCode::Space)) {
        m_boundPipeline = !m_boundPipeline;
    }

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
                if (m_boundPipeline) {
                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_col->get_pipeline());
                } else {
                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_red->get_pipeline());
                }
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
