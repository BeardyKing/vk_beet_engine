
#include <beet/assert.h>
#include <beet/device.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/types.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {}

void Renderer::on_awake() {}
void Renderer::on_update(double deltaTime) {}
void Renderer::on_late_update() {}

void Renderer::on_destroy() {
    log::debug("Renderer destroyed");
}

Renderer::~Renderer() {}

}  // namespace beet
