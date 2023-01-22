#include <beet/asset_manager.h>
#include <beet/engine.h>
#include <beet/renderer.h>

#include <flecs.h>

namespace beet {

AssetManager::AssetManager(Engine& engine, const std::optional<std::weak_ptr<Renderer>>& renderer) : m_engine(engine) {
    if (renderer.has_value() && engine.is_client()) {
        m_renderer = renderer.value();
    }
}

void AssetManager::on_awake() {
    ecs_world_t* ecs = ecs_init();
}

void AssetManager::on_update(double deltaTime) {}

void AssetManager::on_late_update() {}

void AssetManager::on_destroy() {}

AssetManager::~AssetManager() {
    log::debug("AssetManager destroyed");
}

}  // namespace beet
