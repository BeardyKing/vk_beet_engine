#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/resource_manager.h>

#include <beet/gfx/types.h>
#include <beet/gfx/vulkan_pipeline.h>
#include <beet/gfx/vulkan_texture.h>

namespace beet {

ResourceManager::ResourceManager(Engine& engine) : m_engine(engine) {
    s_resourceManager = std::ref(*this);
}

void ResourceManager::on_awake() {
    load_resources_manual();
}

void ResourceManager::load_resources_manual() {
    if (m_engine.is_client()) {
        load_manual_textures();
        generate_pipelines();
    }
}

void ResourceManager::load_manual_textures() {
    load_texture("../res/textures/viking_room.png");
}

void ResourceManager::generate_pipelines() {
    Renderer& renderer = Renderer::get_renderer().value().get();
    m_pipelines[(size_t)gfx::PipelineType::Lit] = renderer.generate_lit_pipeline();
}

std::optional<std::reference_wrapper<ResourceManager>> ResourceManager::get_resource_manager() {
    BEET_ASSERT_MESSAGE(s_resourceManager.has_value(), "AssetManager does not exist")
    return s_resourceManager;
}

std::shared_ptr<gfx::Texture> ResourceManager::load_texture_internal(const std::string& path) {
    Renderer& renderer = Renderer::get_renderer().value().get();

    auto iterator = m_textures.find(path);
    if (iterator != m_textures.end()) {
        return m_textures[path];
    }

    auto texture = std::make_shared<gfx::Texture>();
    texture->rawImage = AssetLoader::load_image(path);

    renderer.upload_texture(*texture);
    renderer.create_image_view(*texture);

    m_textures[path] = texture;

    return texture;
}

std::shared_ptr<gfx::Mesh> ResourceManager::load_mesh_internal(const std::string& path) {
    Renderer& renderer = Renderer::get_renderer().value().get();

    auto iterator = m_meshes.find(path);
    if (iterator != m_meshes.end()) {
        return m_meshes[path];
    }

    auto mesh = std::make_shared<gfx::Mesh>(AssetLoader::load_model(path));
    renderer.upload_mesh(*mesh);

    m_meshes[path] = mesh;
    return mesh;
}

void ResourceManager::free_textures_internal() {
    Renderer& renderer = Renderer::get_renderer().value().get();

    for (auto& texture : m_textures) {
        renderer.destroy_texture(*texture.second);
        renderer.destroy_image_view(*texture.second);
    }
    m_textures.clear();
}

void ResourceManager::free_meshes_internal() {
    Renderer& renderer = Renderer::get_renderer().value().get();

    for (auto& mesh : m_meshes) {
        renderer.destroy_mesh(*mesh.second);
    }
    m_meshes.clear();
}

std::shared_ptr<gfx::VulkanPipeline> ResourceManager::get_pipeline_internal(const gfx::PipelineType& type) {
    return m_pipelines[(size_t)type];
}

void ResourceManager::free_pipelines_internal() {
    std::fill(m_pipelines.begin(), m_pipelines.end(), std::shared_ptr<gfx::VulkanPipeline>());
}

void ResourceManager::free_textures() {
    return ResourceManager::get_resource_manager().value().get().free_textures_internal();
}

void ResourceManager::free_meshes() {
    return ResourceManager::get_resource_manager().value().get().free_meshes_internal();
}

void ResourceManager::free_pipelines() {
    return ResourceManager::get_resource_manager().value().get().free_pipelines_internal();
}

std::shared_ptr<gfx::VulkanPipeline> ResourceManager::get_pipeline(const gfx::PipelineType& type) {
    return ResourceManager::get_resource_manager().value().get().get_pipeline_internal(type);
}

std::shared_ptr<gfx::Texture> ResourceManager::load_texture(const std::string& path) {
    return ResourceManager::get_resource_manager().value().get().load_texture_internal(path);
}

std::shared_ptr<gfx::Mesh> ResourceManager::load_mesh(const std::string& path) {
    return ResourceManager::get_resource_manager().value().get().load_mesh_internal(path);
}

void ResourceManager::on_update(double deltaTime) {}
void ResourceManager::on_late_update() {}
void ResourceManager::on_destroy() {}

ResourceManager::~ResourceManager() {
    log::debug("AssetManager destroyed");
}

}  // namespace beet
