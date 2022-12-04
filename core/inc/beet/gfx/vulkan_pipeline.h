#pragma once

#include <vulkan/vulkan.h>
#include <vector>

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
    void build(const VertexInputDescription& vertexDescription);
    VkPipeline get_pipeline() { return m_pipeline; }

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

   private:
    VkPipeline m_pipeline;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx