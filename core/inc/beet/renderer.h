#pragma once

#include <beet/log.h>
#include <beet/subsystem.h>
#include <beet/types.h>

namespace beet {
class Engine;
}  // namespace beet

namespace beet {

class Renderer : public Subsystem {
   public:
    explicit Renderer(Engine& engine);
    ~Renderer();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

   private:
    Engine& m_engine;
};

}  // namespace beet
