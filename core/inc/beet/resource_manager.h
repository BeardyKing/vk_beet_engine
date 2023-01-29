#pragma once

#include <map>
#include <optional>
#include <array>

#include <flecs.h>

#include <beet/gfx/vulkan_pipeline.h>
#include <beet/gfx/types.h>
#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/types.h>

namespace beet {
class Engine;
class Renderer;
namespace gfx {
struct Texture;
struct Mesh;
}  // namespace gfx
}  // namespace beet

namespace beet {

class ResourceManager : public Subsystem {
   public:
    explicit ResourceManager(Engine& engine);
    ~ResourceManager();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    static std::optional<std::reference_wrapper<ResourceManager>> get_resource_manager();
    static std::shared_ptr<gfx::Texture> load_texture(const std::string& path);
    static std::shared_ptr<gfx::Mesh> load_mesh(const std::string& path);
    static std::shared_ptr<gfx::VulkanPipeline> get_pipeline(const gfx::PipelineTypes& type);
    static void free_textures();
    static void free_meshes();
    static void free_pipelines();

   private:
    void load_resources_manual();
    void load_manual_textures();
    void generate_pipelines();

    std::shared_ptr<gfx::Texture> load_texture_internal(const std::string& path);
    std::shared_ptr<gfx::Mesh> load_mesh_internal(const std::string& path);
    std::shared_ptr<gfx::VulkanPipeline> get_pipeline_internal(const gfx::PipelineTypes& type);

    void free_textures_internal();
    void free_meshes_internal();
    void free_pipelines_internal();

   private:
    Engine& m_engine;

    std::map<std::string, std::shared_ptr<gfx::Texture>> m_textures;
    std::map<std::string, std::shared_ptr<gfx::Mesh>> m_meshes;
    std::array<std::shared_ptr<gfx::VulkanPipeline>, (size_t)gfx::PipelineTypes::LAST> m_pipelines;

    inline static std::optional<std::reference_wrapper<ResourceManager>> s_resourceManager = std::nullopt;
};

}  // namespace beet
