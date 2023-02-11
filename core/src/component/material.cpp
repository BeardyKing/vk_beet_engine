#include <beet/component/material.h>

#include <beet/gfx/types.h>
#include <beet/gfx/vulkan_descriptors.h>
#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_initializers.h>
#include <beet/gfx/vulkan_pipeline.h>

#include <beet/assert.h>
#include <beet/log.h>
#include <beet/renderer.h>
#include <beet/resource_manager.h>

namespace beet {

Material::Material() : m_pipelineType(gfx::PipelineType::Invalid) {
    init_internal();
}

Material::Material(gfx::PipelineType type) : m_pipelineType(type) {
    init_internal();
}

void Material::init_internal() {
    Renderer& renderer = Renderer::get_renderer().value().get();
    auto device = renderer.get_vulkan_device()->get_device();
    auto descriptorPools = renderer.get_vulkan_descriptor()->get_descriptor_pool();
    auto textureDescriptorSet = renderer.get_vulkan_descriptor()->get_texture_descriptor_set();

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPools;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &textureDescriptorSet;
    vkAllocateDescriptorSets(device, &allocInfo, &m_textureSets);
}

Material::~Material() {
    log::debug("Destroyed Material");
}

void Material::on_awake() {}
void Material::on_update(double deltaTime) {}
void Material::on_late_update() {}
void Material::on_destroy() {}

std::shared_ptr<gfx::VulkanPipeline> Material::get_vulkan_pipeline() const {
    return ResourceManager::get_pipeline(m_pipelineType);
}

void Material::set_pipeline_type(const gfx::PipelineType& type) {
    m_pipelineType = type;
}

// TODO:    This code is needed to update the material descriptors whenever there is a change resource
//          i.e texture/value change. it is likely in release state that most resources would be static
//          but this is not the case when we are using the editor as we update resource values quite often.
//          any all cases we would want to defer updating the resource until the end of the fame so that
//          we don't cause any hitching when updating the descriptors :)
void Material::update_material_descriptor() const {
    Renderer& renderer = Renderer::get_renderer().value().get();
    auto device = renderer.get_vulkan_device()->get_device();
    auto linearSampler = renderer.get_linear_sampler();

    std::array<VkWriteDescriptorSet, 5> descriptors{};
    {
        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = linearSampler;
        imageBufferInfo.imageView = m_albedo->imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptors[0] = gfx::init::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_textureSets,
                                                           &imageBufferInfo, 0);
    }
    {
        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = linearSampler;
        imageBufferInfo.imageView = m_normal->imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptors[1] = gfx::init::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_textureSets,
                                                           &imageBufferInfo, 1);
    }
    {
        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = linearSampler;
        imageBufferInfo.imageView = m_metallic->imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptors[2] = gfx::init::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_textureSets,
                                                           &imageBufferInfo, 2);
    }
    {
        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = linearSampler;
        imageBufferInfo.imageView = m_roughness->imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptors[3] = gfx::init::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_textureSets,
                                                           &imageBufferInfo, 3);
    }
    {
        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = linearSampler;
        imageBufferInfo.imageView = m_occlusion->imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptors[4] = gfx::init::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_textureSets,
                                                           &imageBufferInfo, 4);
    }

    vkUpdateDescriptorSets(device, (uint32_t)descriptors.size(), descriptors.data(), 0, nullptr);
}

void Material::bind_descriptor(const VkCommandBuffer& cmd) const {
    BEET_ASSERT_MESSAGE(m_textureSets != VK_NULL_HANDLE, "Error: Material texture set is invalid.");
    auto layout = get_vulkan_pipeline()->get_pipeline_layout();
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1, &m_textureSets, 0, nullptr);
}

void Material::set_normal_texture(const std::shared_ptr<gfx::Texture>& texture) {
    m_normal = texture;
}

void Material::set_albedo_texture(const std::shared_ptr<gfx::Texture>& texture) {
    m_albedo = texture;
}

void Material::set_metallic_texture(const std::shared_ptr<gfx::Texture>& texture) {
    m_metallic = texture;
}

void Material::set_roughness_texture(const std::shared_ptr<gfx::Texture>& texture) {
    m_roughness = texture;
}

void Material::set_occlusion_texture(const std::shared_ptr<gfx::Texture>& texture) {
    m_occlusion = texture;
}

}  // namespace beet