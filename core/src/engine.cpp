#include <beet/engine.h>

namespace beet {

Engine::Engine() {
    m_windowModule = std::make_shared<Window>(1024, 768, "vk_beetroot", *this);
    m_rendererModule = std::make_shared<Renderer>(*this);
    m_resourceModule = std::make_shared<ResourceManager>(*this);
    m_sceneModule = std::make_shared<Scene>(*this);

    // order dependent
    m_engineModules.emplace_back(m_windowModule);
    m_engineModules.emplace_back(m_rendererModule);
    m_engineModules.emplace_back(m_resourceModule);
    m_engineModules.emplace_back(m_sceneModule);

    for (auto& module : m_engineModules) {
        module->on_awake();
    }
}

void Engine::update_modules() {
    for (auto& module : m_engineModules) {
        module->on_update(m_windowModule->get_delta_time());
    }

    for (auto& module : m_engineModules) {
        module->on_late_update();
    }

    swap_frame();
}

Engine::~Engine() {
    for (auto& module : m_engineModules) {
        module->on_destroy();
    }
}

bool Engine::is_open() {
    return m_windowModule->is_open();
}

void Engine::swap_frame() {
    m_windowModule->swap_frame();
}

bool Engine::is_client() {
    return true;
}

}  // namespace beet