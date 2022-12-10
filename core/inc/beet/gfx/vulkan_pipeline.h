#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <beet/gfx/types.h>
#include <beet/types.h>

namespace beet {
class Renderer;
namespace gfx {
class VulkanShaderModules;
struct VertexInputDescription;
}  // namespace gfx
}  // namespace beet

namespace beet::gfx {
class VulkanPipeline {
   public:
    VulkanPipeline(Renderer& renderer);
    ~VulkanPipeline();

    void add_stages(gfx::VulkanShaderModules& shaderModules);
    void build(const VertexInputDescription& vertexDescription, const VkPushConstantRange& pushConstantRange);
    VkPipeline get_pipeline() { return m_pipeline; }
    VkPipelineLayout get_pipelineLayout() { return m_pipelineLayout; }

   private:
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    VkPipelineRasterizationStateCreateInfo m_rasterizer;
    VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo m_multisampling;
    VkPipelineLayout m_pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo m_depthStencil;

   private:
    VkPipeline m_pipeline;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx