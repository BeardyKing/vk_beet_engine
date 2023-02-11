#pragma once

#include <memory>
#include <array>

#include <beet/gfx/types.h>

#include <beet/subsystem.h>
#include <beet/types.h>

namespace beet::gfx {
class VulkanPipeline;
struct Texture;
}  // namespace beet::gfx

namespace beet {

class Material : public Subsystem {
   public:
    Material();
    explicit Material(gfx::PipelineType type);

    ~Material();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void update_material_descriptor() const;
    void bind_descriptor(const VkCommandBuffer& cmd) const;
    [[nodiscard]] std::shared_ptr<gfx::VulkanPipeline> get_vulkan_pipeline() const;

   private:
    gfx::PipelineType m_pipelineType;

    std::shared_ptr<gfx::Texture> m_albedo;
    std::shared_ptr<gfx::Texture> m_normal;
    std::shared_ptr<gfx::Texture> m_metallic;
    std::shared_ptr<gfx::Texture> m_roughness;
    std::shared_ptr<gfx::Texture> m_occlusion;

    VkDescriptorSet m_textureSets = {VK_NULL_HANDLE};

    vec4 m_albedoColor = vec4(1.0f);
    vec2 m_textureUV = vec2(1.0f);

   public:
    void init_internal();
    void set_pipeline_type(const gfx::PipelineType& type);
    vec4 get_albedo_color() { return m_albedoColor; };
    vec2 get_texture_uv() { return m_textureUV; };

    void set_albedo_texture(const std::shared_ptr<gfx::Texture>& texture); //TODO replace with set (ENUM, TEXTURE);
    void set_normal_texture(const std::shared_ptr<gfx::Texture>& texture); //TODO replace with set (ENUM, TEXTURE);
    void set_metallic_texture(const std::shared_ptr<gfx::Texture>& texture); //TODO replace with set (ENUM, TEXTURE);
    void set_roughness_texture(const std::shared_ptr<gfx::Texture>& texture); //TODO replace with set (ENUM, TEXTURE);
    void set_occlusion_texture(const std::shared_ptr<gfx::Texture>& texture); //TODO replace with set (ENUM, TEXTURE);
    void set_albedo_color(const vec4& color) { m_albedoColor = color; }
    void set_texture_uv(const vec2& uv) { m_textureUV = uv; }
};

}  // namespace beet