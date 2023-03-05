#include <beet/renderer.h>

#include <beet/gfx/vulkan_buffer.h>
#include <beet/gfx/vulkan_command_buffer.h>
#include <beet/gfx/vulkan_command_queue.h>
#include <beet/gfx/vulkan_descriptors.h>
#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_initializers.h>
#include <beet/gfx/vulkan_mesh.h>
#include <beet/gfx/vulkan_pipeline.h>
#include <beet/gfx/vulkan_render_pass.h>
#include <beet/gfx/vulkan_shader_modules.h>
#include <beet/gfx/vulkan_swapchain.h>
#include <beet/gfx/vulkan_texture.h>

#include <beet/component/camera.h>
#include <beet/component/material.h>
#include <beet/component/transform.h>

#include <beet/assert.h>
#include <beet/asset_loader.h>
#include <beet/resource_manager.h>
#include <beet/scene.h>
#include <beet/types.h>
#include <beet/window.h>

namespace beet {
struct DynamicViewport {
    VkViewport viewport;
    VkRect2D scissor;
};
}  // namespace beet

namespace beet {

void draw_sky(VkCommandBuffer& cmd);
void draw_lit(VkCommandBuffer& cmd);

Renderer::Renderer() {
    s_renderer = std::ref(*this);

    m_device = std::make_shared<gfx::VulkanDevice>(*this);
    m_buffer = std::make_shared<gfx::VulkanBuffer>(*this);
    m_swapchain = std::make_shared<gfx::VulkanSwapchain>(*this);
    m_commandBuffer = std::make_shared<gfx::VulkanCommandBuffer>(*this);
    m_renderPass = std::make_shared<gfx::VulkanRenderPass>(*this);
    m_commandQueue = std::make_shared<gfx::VulkanCommandQueue>(*this);

    // TODO:    Create sampler class and add to the resource manager.
    VkSamplerCreateInfo samplerInfo = gfx::init::sampler_create_info(VK_FILTER_LINEAR);
    vkCreateSampler(m_device->get_device(), &samplerInfo, nullptr, &m_linearSampler);
}

// TODO: SHOULD BE MANAGED VIA MATERIAL and or SHADER
struct MeshPushConstants {
    glm::vec4 data;
    glm::mat4 render_matrix;
};

std::shared_ptr<gfx::VulkanPipeline> Renderer::generate_lit_pipeline() {
    auto litPipeline = std::make_shared<gfx::VulkanPipeline>(*this);

    auto triRedVertSrc = AssetLoader::load_shader_binary("../res/shaders/lit/standard_lit.vert.spv");
    auto triRedFragSrc = AssetLoader::load_shader_binary("../res/shaders/lit/standard_lit.frag.spv");
    gfx::VulkanShaderModules litShader(*this);
    litShader.load(triRedVertSrc, gfx::ShaderType::Vertex);
    litShader.load(triRedFragSrc, gfx::ShaderType::Fragment);

    {
        gfx::VertexInputDescription vertexDescription = gfx::Vertex::get_vertex_description();
        litPipeline->add_stages(litShader);

        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(MeshPushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        litPipeline->build(vertexDescription, pushConstant);
    }

    return litPipeline;
}

std::shared_ptr<gfx::VulkanPipeline> Renderer::generate_sky_pipeline() {
    auto litPipeline = std::make_shared<gfx::VulkanPipeline>(*this);

    auto vertSrc = AssetLoader::load_shader_binary("../res/shaders/sky/octahedral_sky.vert.spv");
    auto fragSrc = AssetLoader::load_shader_binary("../res/shaders/sky/octahedral_sky.frag.spv");
    gfx::VulkanShaderModules skyShader(*this);
    skyShader.load(vertSrc, gfx::ShaderType::Vertex);
    skyShader.load(fragSrc, gfx::ShaderType::Fragment);

    {
        gfx::VertexInputDescription vertexDescription = gfx::Vertex::get_vertex_description();
        litPipeline->add_stages(skyShader);

        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(MeshPushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        // TODO: disable depth write for sky shader.

        vertexDescription.flagWriteDepth = false;

        litPipeline->build(vertexDescription, pushConstant);
    }

    return litPipeline;
}

void Renderer::on_awake() {
    m_descriptors = std::make_shared<gfx::VulkanDescriptors>(*this);
}

void Renderer::recreate_swap_chain() {
    Window& window = Window::get_window().value().get();
    vec2i size{0};
    while (!size.x || !size.y) {
        window.get_framebuffer_size(size);
        window.wait_events();
    }

    wait_idle();

    m_commandBuffer->recreate();
    m_swapchain->recreate();
    m_renderPass->recreate();
}

void Renderer::on_update(double deltaTime) {
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
    set_clear_info(info);

    auto dynamicViewport = update_viewport_scissor();

    update_camera_descriptor();
    update_scene_descriptor();

    auto cmd = m_commandBuffer->get_main_command_buffer();
    {
        m_commandBuffer->begin_recording();
        {
            vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
            {
                {
                    vkCmdSetViewport(cmd, 0, 1, &dynamicViewport.viewport);
                    vkCmdSetScissor(cmd, 0, 1, &dynamicViewport.scissor);
                }

                draw_sky(cmd);
                draw_lit(cmd);
            }
            vkCmdEndRenderPass(cmd);
        }
        m_commandBuffer->end_recording();
    }

    m_commandQueue->add_command(cmd);
    m_commandQueue->submit();
    m_swapchain->present();
    m_commandBuffer->next_frame();
}

void draw_sky(VkCommandBuffer& cmd) {
    auto& renderer = Renderer::get_renderer().value().get();
    auto litPipeline = ResourceManager::get_pipeline(gfx::PipelineType::Sky).get();
    auto pipeline = litPipeline->get_pipeline();
    auto pipelineLayout = litPipeline->get_pipeline_layout();
    auto globalDescriptor = renderer.get_vulkan_command_buffer()->get_global_descriptor();

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    {
        // bind global graphics descriptors { camera, scene }
        uint32_t uniform_offset = Renderer::pad_uniform_buffer_size(sizeof(gfx::GPUSceneData), "GPUSceneData") *
                                  renderer.get_vulkan_command_buffer()->get_current_frame_index();
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &globalDescriptor, 1,
                                &uniform_offset);
    }

    auto world = Scene::get_world().get();
    world->each([cmd](flecs::entity e, const Transform& transform, const Material& material,
                      const std::shared_ptr<gfx::Mesh>& mesh, const gfx::SkyPipeline pipelineTags) {
        auto layout = material.get_vulkan_pipeline()->get_pipeline_layout();
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &mesh->vertexBuffer.buffer, &offset);

        // TODO when mips supported add skybox mip selection via "data"
        MeshPushConstants tmpConstants{
            glm::vec4{0},
            glm::mat4(1.0f),
        };

        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &tmpConstants);
        vkCmdDraw(cmd, mesh->vertices.size(), 1, 0, 0);
    });
}

void draw_lit(VkCommandBuffer& cmd) {
    auto& renderer = Renderer::get_renderer().value().get();
    auto litPipeline = ResourceManager::get_pipeline(gfx::PipelineType::Lit).get();
    auto pipeline = litPipeline->get_pipeline();
    auto pipelineLayout = litPipeline->get_pipeline_layout();
    auto globalDescriptor = renderer.get_vulkan_command_buffer()->get_global_descriptor();

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    {
        // bind global graphics descriptors { camera, scene }
        uint32_t uniform_offset = Renderer::pad_uniform_buffer_size(sizeof(gfx::GPUSceneData), "GPUSceneData") *
                                  renderer.get_vulkan_command_buffer()->get_current_frame_index();
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &globalDescriptor, 1,
                                &uniform_offset);
    }

    auto world = Scene::get_world().get();
    world->each([cmd](flecs::entity e, const Transform& transform, const Material& material,
                      const std::shared_ptr<gfx::Mesh>& mesh, const gfx::LitPipeline pipelineTag) {
        auto layout = material.get_vulkan_pipeline()->get_pipeline_layout();

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &mesh->vertexBuffer.buffer, &offset);

        MeshPushConstants tmpConstants{
            glm::vec4{0},
            transform.get_model_matrix(),
        };

        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &tmpConstants);

        material.bind_descriptor(cmd);

        vkCmdDraw(cmd, mesh->vertices.size(), 1, 0, 0);
    });
}

void Renderer::update_scene_descriptor() {
    auto allocator = m_buffer->get_allocator();
    auto allocatedBuffer = m_commandBuffer->get_scene_data_buffer();
    uint32_t frameIdx = m_commandBuffer->get_current_frame_index();

    //===SCENE::UPDATE_GLOBAL_DESCRIPTOR===//
    // TODO: only update this buffer for 2 frames after the data has changed.
    char* sceneData;
    vmaMapMemory(allocator, allocatedBuffer.allocation, (void**)&sceneData);
    sceneData += pad_uniform_buffer_size(sizeof(gfx::GPUSceneData), "GPUSceneData") * frameIdx;
    memcpy(sceneData, &m_sceneData, sizeof(gfx::GPUSceneData));
    vmaUnmapMemory(allocator, allocatedBuffer.allocation);
}

void Renderer::update_camera_descriptor() {
    auto aspectRatio = Window::get_aspect_ratio();
    auto buffer = m_buffer;
    auto commandBuffer = m_commandBuffer;
    auto world = Scene::get_world().get();

    world->each(
        [aspectRatio, buffer, commandBuffer](flecs::entity e, const Transform& transform, const Camera& camera) {
            const vec3 position = transform.get_position();
            const mat4 projection = camera.get_projection(aspectRatio);
            const mat4 view = lookAt(position, camera.get_look_target(), transform.up());
            const mat4 viewProjection = projection * view;

            const gfx::GPUCameraData camData{projection, view, viewProjection, vec4(position, 1.0f)};

            //===CAMERA::UPDATE_GLOBAL_DESCRIPTOR===//
            void* data;
            auto allocator = buffer->get_allocator();
            auto cameraBuffer = commandBuffer->get_camera_buffer();
            vmaMapMemory(allocator, cameraBuffer.allocation, &data);
            memcpy(data, &camData, sizeof(gfx::GPUCameraData));
            vmaUnmapMemory(allocator, cameraBuffer.allocation);
        });
}

void Renderer::set_clear_info(VkRenderPassBeginInfo& info) {
    VkClearValue clearValue;
    VkClearValue depthClear;
    clearValue.color = {{0.5f, 0.092, 0.167f, 1.0f}};
    depthClear.depthStencil.depth = 1.0f;

    info.clearValueCount = 1;
    info.pClearValues = &clearValue;
    info.clearValueCount = 2;
    static VkClearValue clearValues[2] = {clearValue, depthClear};
    info.pClearValues = &clearValues[0];
}

DynamicViewport Renderer::update_viewport_scissor() {
    vec2u windowSize = Window::get_size();
    VkExtent2D extent = {static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)};

    return DynamicViewport{
        {0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f},
        {{0, 0}, {extent}},
    };
}

void Renderer::on_late_update() {}

void Renderer::on_destroy() {}

// TODO:    VK UTIL
//          MOVE FUNCTIONS TO UTIL CLASS
void Renderer::destroy_texture(gfx::Texture& texture) {
    m_buffer->destroy_texture(texture);
}

void Renderer::upload_texture(gfx::Texture& texture) {
    m_buffer->upload_texture(texture);
}

void Renderer::create_image_view(gfx::Texture& texture) {
    auto device = get_vulkan_device()->get_device();
    VkImageViewCreateInfo imageInfo =
        gfx::init::imageview_create_info(texture.rawImage.format, texture.image.image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(device, &imageInfo, nullptr, &texture.imageView);
}

void Renderer::destroy_image_view(gfx::Texture& texture) {
    auto device = get_vulkan_device()->get_device();
    vkDestroyImageView(device, texture.imageView, nullptr);
}

void Renderer::upload_mesh(gfx::Mesh& mesh) {
    m_buffer->upload_mesh(mesh);
}

void Renderer::destroy_mesh(gfx::Mesh& mesh) {
    m_buffer->destroy_mesh(mesh);
}
// TODO:    VK UTIL

std::optional<std::reference_wrapper<Renderer>> Renderer::get_renderer() {
    BEET_ASSERT_MESSAGE(s_renderer.has_value(), "Renderer does not exist")
    return s_renderer;
}

size_t Renderer::pad_uniform_buffer_size(size_t originalSize,
                                         const std::string& bufferName,
                                         bool verbose /* = false */) {
    auto device = Renderer::get_renderer().value().get().get_vulkan_device().get();
    size_t minUboAlignment = device->get_gpu_properties().limits.minUniformBufferOffsetAlignment;
    size_t alignedSize = originalSize;
    if (minUboAlignment > 0) {
        alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
    }
    if (verbose) {
        log::warn("free space in buffer [{}] aligning up to [{}] from [{}]", bufferName, alignedSize, originalSize);
    }
    return alignedSize;
}

Renderer::~Renderer() {
    // TODO:    This is a bit of a hack we just render out as many frames as we have buffered command buffers
    for (auto& frame : get_vulkan_command_buffer()->get_frame_data()) {
        render_sync();
        m_commandBuffer->reset();
        m_swapchain->acquire_next_image();
        m_commandBuffer->next_frame();
    }

    ResourceManager::free_textures();
    ResourceManager::free_meshes();
    ResourceManager::free_pipelines();

    vkDestroySampler(m_device->get_device(), m_linearSampler, nullptr);

    log::debug("Renderer destroyed");
}
void Renderer::wait_idle() {
    m_device->wait_idle();
}
void Renderer::render_sync() {
    m_renderPass->sync();
}
}  // namespace beet
