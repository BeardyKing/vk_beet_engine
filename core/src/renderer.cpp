#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_initializers.h>
#include <beet/gfx/vulkan_texture.h>

#include <beet/assert.h>
#include <beet/asset_loader.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/types.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    m_device = std::make_shared<gfx::VulkanDevice>(*this);
    m_buffer = std::make_shared<gfx::VulkanBuffer>(*this);
    m_swapchain = std::make_shared<gfx::VulkanSwapchain>(*this);
    m_commandBuffer = std::make_shared<gfx::VulkanCommandBuffer>(*this);
    m_renderPass = std::make_shared<gfx::VulkanRenderPass>(*this);
    m_commandQueue = std::make_shared<gfx::VulkanCommandQueue>(*this);
    m_descriptors = std::make_shared<gfx::VulkanDescriptors>(*this);
    m_pipelineMesh = std::make_shared<gfx::VulkanPipeline>(*this);
}

// TODO: SHOULD BE MANAGED VIA MATERIAL and or SHADER
struct MeshPushConstants {
    glm::vec4 data;
    glm::mat4 render_matrix;
};

void Renderer::on_awake() {
    // RES:TODO:    REPLACE WITH RESOURCE MANAGER
    {
        // RES:IMAGE:TODO:  UPLOADING IMAGE DATA TO GPU SHOULD BE DONE VIA RESOURCE MANAGER
        m_loadedTexture.rawImage = AssetLoader::load_image("../res/textures/viking_room.png");
        m_buffer->upload_texture(m_loadedTexture);

        // BUILD IMAGE VIEW
        VkImageViewCreateInfo imageInfo = gfx::init::imageview_create_info(
            m_loadedTexture.rawImage.format, m_loadedTexture.image.image, VK_IMAGE_ASPECT_COLOR_BIT);
        vkCreateImageView(get_device(), &imageInfo, nullptr, &m_loadedTexture.imageView);
    }
    {
        // RES:MESH:TODO:   UPLOADING MESH DATA TO GPU SHOULD BE DONE VIA RESOURCE MANAGER
        m_loadedMesh = AssetLoader::load_model("../res/misc/viking_room.obj");
        m_buffer->upload_mesh(m_loadedMesh);
    }
    {
        // RES:SHADER
        auto triRedVertSrc = AssetLoader::read_file("../res/shaders/standard_unlit.vert.spv");
        auto triRedFragSrc = AssetLoader::read_file("../res/shaders/standard_unlit.frag.spv");
        gfx::VulkanShaderModules redTriangleShader(*this);
        redTriangleShader.load(triRedVertSrc, gfx::ShaderType::Vertex);
        redTriangleShader.load(triRedFragSrc, gfx::ShaderType::Fragment);
        {
            // PIPELINE
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

void Renderer::recreate_swap_chain() {
    auto window = m_engine.get_window_module().lock();
    vec2i size{0};
    while (!size.x || !size.y) {
        window->get_framebuffer_size(size);
        window->wait_events();
    }

    wait_idle();

    m_commandBuffer->recreate();
    m_swapchain->recreate();
    m_renderPass->recreate();
}

void Renderer::on_update(double deltaTime) {
    VkClearValue clearValue;
    VkClearValue depthClear;
    clearValue.color = {{0.5f, 0.092, 0.167f, 1.0f}};
    depthClear.depthStencil.depth = 1.f;

    render_sync();
    m_commandBuffer->reset();

    auto result = m_swapchain->acquire_next_image();
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        BEET_ASSERT_MESSAGE(BEET_FALSE, "failed to acquire swap chain image!");
        return;
    }

    auto info = m_renderPass->create_begin_info();
    info.clearValueCount = 1;
    info.pClearValues = &clearValue;
    info.clearValueCount = 2;
    VkClearValue clearValues[] = {clearValue, depthClear};
    info.pClearValues = &clearValues[0];

    // TODO: This is painful and should really be managed by some class / components
    //       i.e. Camera/Transform/UpdateGlobalDescriptor
    //===MESH LOCAL===//
    glm::mat4 model = glm::rotate(glm::mat4{1.0f}, glm::radians(m_timePassed * 20.0f), glm::vec3(0, 1, 0));
    glm::mat4 meshMatrix = model;

    //===CAMERA===//
    glm::vec3 camPos = {0.f, -1.0f, -3.f};
    glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
    projection[1][1] *= -1;

    gfx::GPUCameraData camData{};
    camData.proj = projection;
    camData.view = view;
    camData.viewProj = projection * view;

    //===CAMERA::UPDATE_GLOBAL_DESCRIPTOR===//
    void* data;
    auto allocator = m_buffer->get_allocator();
    auto cameraBuffer = m_commandBuffer->get_camera_buffer();
    vmaMapMemory(allocator, cameraBuffer.allocation, &data);
    memcpy(data, &camData, sizeof(gfx::GPUCameraData));
    vmaUnmapMemory(allocator, cameraBuffer.allocation);
    //===EXIT HELL===//

    auto cmd = m_commandBuffer->get_main_command_buffer();
    {
        m_commandBuffer->begin_recording();
        {
            vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
            {
                // TODO: We should only rebind our pipeline when it changes
                //       (There is only 1 pipeline in use currently)
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineMesh->get_pipeline());
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineMesh->get_pipelineLayout(), 0,
                                        1, &get_global_descriptor(), 0, nullptr);
                // TODO:END

                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(cmd, 0, 1, &m_loadedMesh.vertexBuffer.buffer, &offset);

                MeshPushConstants tmpConstants{};
                tmpConstants.render_matrix = meshMatrix;

                vkCmdPushConstants(cmd, m_pipelineMesh->get_pipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                                   sizeof(MeshPushConstants), &tmpConstants);

                vkCmdDraw(cmd, m_loadedMesh.vertices.size(), 1, 0, 0);
            }
            vkCmdEndRenderPass(cmd);
        }
        m_commandBuffer->end_recording();
    }

    m_commandQueue->add_command(cmd);
    m_commandQueue->submit();
    m_swapchain->present();
    m_commandBuffer->next_frame();

    m_timePassed += (float)deltaTime;
}

void Renderer::on_late_update() {}

void Renderer::on_destroy() {}

Renderer::~Renderer() {
    // TODO:    This is a bit of a hack we just render out as many frames as we have buffered command buffers
    for (auto& frame : get_frame_data()) {
        render_sync();
        m_commandBuffer->reset();
        m_swapchain->acquire_next_image();
        m_commandBuffer->next_frame();
    }

    m_buffer->destroy_mesh(m_loadedMesh);

    m_buffer->destroy_texture(m_loadedTexture);
    vkDestroyImageView(get_device(), m_loadedTexture.imageView, nullptr);

    log::debug("Renderer destroyed");
}

}  // namespace beet
