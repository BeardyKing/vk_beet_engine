#pragma once

#include <beet/subsystem.h>
#include <optional>
#include <memory>

#include "flecs.h"

namespace beet {
class Engine;
}  // namespace beet

namespace beet {

class Scene : public Subsystem {
   public:
    explicit Scene(Engine& engine);
    ~Scene();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    static std::optional<std::reference_wrapper<Scene>> get_scene();
    static std::shared_ptr<flecs::world> get_world();

   private:
    std::shared_ptr<flecs::world> get_world_internal();
    void build_scene_manual();

   private:
    Engine& m_engine;
    inline static std::optional<std::reference_wrapper<Scene>> s_scene = std::nullopt;

    std::shared_ptr<flecs::world> m_world;
};

}  // namespace beet