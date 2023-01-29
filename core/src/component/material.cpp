#include <beet/component/material.h>

#include <beet/gfx/types.h>
#include <beet/log.h>
#include <beet/resource_manager.h>

#include <utility>

namespace beet {

Material::Material(const gfx::PipelineType& type) : m_pipelineType(type) {}

Material::~Material() {
    log::debug("Destroyed Material");
}

void Material::on_awake() {}
void Material::on_update(double deltaTime) {}
void Material::on_late_update() {}
void Material::on_destroy() {}

void Material::set_albedo_texture(const std::shared_ptr<gfx::Texture>& texture) {
    m_albedo = texture;
}

std::shared_ptr<gfx::VulkanPipeline> Material::get_vulkan_pipeline() {
    return ResourceManager::get_pipeline(m_pipelineType);
}
}  // namespace beet