#pragma once

#include <optional>

#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/types.h>

namespace beet {
class Engine;
class Renderer;
}  // namespace beet

namespace beet {

class AssetManager : public Subsystem {
   public:
    explicit AssetManager(Engine& engine, const std::optional<std::weak_ptr<Renderer>>& renderer);
    ~AssetManager();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    Engine& get_engine() { return m_engine; }

   private:
    Engine& m_engine;
    std::weak_ptr<Renderer> m_renderer;
};

}  // namespace beet
