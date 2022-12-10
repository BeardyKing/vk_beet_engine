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
    m_buffer = std::make_shared<gfx::VulkanBuffer>(*this);

    m_pipelineMesh = std::make_shared<gfx::VulkanPipeline>(*this);
}

// TODO: SHOULD BE MANAGED VIA MATERIAL and or SHADER
struct MeshPushConstants {
    glm::vec4 data;
    glm::mat4 render_matrix;
};

void Renderer::on_awake() {
    //  RESOURCES: TODO: REPLACE WITH RESOURCE MANAGER
    {
        //  RES:MESH
        m_triangle.vertices.resize(3);
        m_triangle.vertices[0].position = {1.f, 1.f, 0.0f};
        m_triangle.vertices[1].position = {-1.f, 1.f, 0.0f};
        m_triangle.vertices[2].position = {0.f, -1.f, 0.0f};
        m_triangle.vertices[0].color = {1.f, 0.f, 0.0f};
        m_triangle.vertices[1].color = {0.f, 1.f, 0.0f};
        m_triangle.vertices[2].color = {0.f, 0.f, 1.0f};

        //  TODO: UPLOADING MESH DATA TO GPU SHOULD BE DONE VIA RESOURCE MANAGER
        m_buffer->upload_mesh_immediate(m_triangle);
    }
    {
        //  RES:SHADER
        auto triRedVertSrc = AssetLoader::read_file("../res/shaders/tri_mesh_shader.vert.spv");
        auto triRedFragSrc = AssetLoader::read_file("../res/shaders/tri_mesh_shader.frag.spv");
        gfx::VulkanShaderModules redTriangleShader(*this);
        redTriangleShader.load(triRedVertSrc, gfx::ShaderType::Vertex);
        redTriangleShader.load(triRedFragSrc, gfx::ShaderType::Fragment);
        {
            //  PIPELINE
            gfx::VertexInputDescription vertexDescription = gfx::Vertex::get_vertex_description();
            m_pipelineMesh->add_stages(redTriangleShader);

            VkPushConstantRange pushConstant;
            pushConstant.offset = 0;
            pushConstant.size = sizeof(MeshPushConstants);
            pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            m_pipelineMesh->build(vertexDescription, pushConstant);
        }
    }
}

void Renderer::on_update(double deltaTime) {
    VkClearValue clearValue;
    clearValue.color = {{0.5f, 0.092, 0.167f, 1.0f}};
    m_timePassed += deltaTime;

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
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineMesh->get_pipeline());
                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(cmd, 0, 1, &m_triangle.vertexBuffer.buffer, &offset);

                glm::vec3 camPos = {0.f, 0.f, -2.f};
                glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
                glm::mat4 projection = glm::perspective(glm::radians(90.f), 1700.f / 900.f, 0.1f, 200.0f);
                projection[1][1] *= -1;
                glm::mat4 model = glm::rotate(glm::mat4{1.0f}, glm::radians(m_timePassed * 20.0f), glm::vec3(0, 1, 0));
                glm::mat4 mesh_matrix = projection * view * model;

                MeshPushConstants tmpConstants{};
                tmpConstants.render_matrix = mesh_matrix;
                vkCmdPushConstants(cmd, m_pipelineMesh->get_pipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                                   sizeof(MeshPushConstants), &tmpConstants);

                vkCmdDraw(cmd, m_triangle.vertices.size(), 1, 0, 0);
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
    m_buffer->destroy_mesh(m_triangle);
    log::debug("Renderer destroyed");
}

}  // namespace beet
