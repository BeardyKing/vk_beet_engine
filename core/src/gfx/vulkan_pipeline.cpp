#include <beet/gfx/vulkan_pipeline.h>

#include <beet/gfx/vulkan_descriptors.h>
#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_initializers.h>
#include <beet/gfx/vulkan_mesh.h>
#include <beet/gfx/vulkan_render_pass.h>
#include <beet/gfx/vulkan_shader_modules.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanPipeline::VulkanPipeline(Renderer& renderer) : m_renderer(renderer) {}

VulkanPipeline::~VulkanPipeline() {
    auto device = m_renderer.get_vulkan_device()->get_device();

    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
    log::debug("VulkanPipeline destroyed");
}

void VulkanPipeline::add_stages(gfx::VulkanShaderModules& shaderModules) {
    auto vertexModule = shaderModules.get_module(ShaderType::Vertex);
    auto fragmentModule = shaderModules.get_module(ShaderType::Fragment);

    if (vertexModule != VK_NULL_HANDLE) {
        m_shaderStages.emplace_back(init::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertexModule));
    }
    if (fragmentModule != VK_NULL_HANDLE) {
        m_shaderStages.emplace_back(
            init::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentModule));
    }
}

void VulkanPipeline::build(const VertexInputDescription& vertexDescription,
                           const VkPushConstantRange& pushConstantRange) {
    auto device = m_renderer.get_vulkan_device()->get_device();
    auto renderPass = m_renderer.get_vulkan_render_pass()->get_render_pass();

    auto size = Window::get_size();
    VkExtent2D extent = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};

    //===BUILD PIPELINE INFO===//
    VkDescriptorSetLayout descriptorLayouts[2] = {m_renderer.get_vulkan_descriptor()->get_global_descriptor_set(),
                                                  m_renderer.get_vulkan_descriptor()->get_texture_descriptor_set()};
    VkPipelineLayoutCreateInfo pipeline_layout_info = init::pipeline_layout_create_info();
    pipeline_layout_info.pPushConstantRanges = &pushConstantRange;
    pipeline_layout_info.pushConstantRangeCount = 1;  // FIXME:   Only supports 1 stage at this time.
    pipeline_layout_info.setLayoutCount = 2;
    pipeline_layout_info.pSetLayouts = descriptorLayouts;

    auto pipelineInfoResult = vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &m_pipelineLayout);
    BEET_ASSERT_MESSAGE(pipelineInfoResult == VK_SUCCESS, "failed to create pipeline")

    //===BUILD VERTEX BUFFER===//
    m_vertexInputInfo = init::vertex_input_state_create_info();
    if (!vertexDescription.attributes.empty()) {
        m_vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
        m_vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
    }
    if (!vertexDescription.bindings.empty()) {
        m_vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
        m_vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();
    }

    //===BUILD ASSEMBLY===//
    m_inputAssembly = init::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    //===BUILD RASTERIZATION===//
    m_rasterizer = init::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

    //===BUILD MULTISAMPLING===//
    m_multisampling = init::multisampling_state_create_info();

    //===BUILD BLEND ATTACHMENT===//
    m_colorBlendAttachment = init::color_blend_attachment_state();

    //===VIEWPORT===//
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    //===BLENDING===//
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &m_colorBlendAttachment;

    //===BUILD DEPTH===//
    // FIXME:   ONLY SUPPORTING DEPTH COMPARE (UNTIL PARTICLE SYSTEM IMPL)
    m_depthStencil = init::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

    //===BUILD PIPELINE===//
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;

    pipelineInfo.stageCount = m_shaderStages.size();
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &m_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizer;
    pipelineInfo.pMultisampleState = &m_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &m_depthStencil;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pDynamicState = &dynamicState;

    auto result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Failed to create graphics pipeline");

    if (result != VK_SUCCESS) {
        // TODO: If we fail to build the graphics pipeline in release we should try and recover
        //       if we do not manage to recover we should revert our settings to the minimum supported.

        log::critical("VK ERR: Failed to create graphics pipeline");
        m_pipeline = VK_NULL_HANDLE;
    }
}

}  // namespace beet::gfx