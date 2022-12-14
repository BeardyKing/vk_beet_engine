#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <beet/device.h>
#include <beet/renderer.h>
#include <beet/window.h>

namespace beet {
class EditorWidgets;
}

namespace beet {
class Engine {
   public:
    Engine();
    ~Engine();

    void update_modules();
    bool is_open();

    std::weak_ptr<Renderer> get_renderer_module() { return m_rendererModule; }
    std::weak_ptr<Window> get_window_module() { return m_windowModule; }

    inline void swap_frame();

   private:
    std::vector<std::shared_ptr<Subsystem>> m_engineModules;

    std::shared_ptr<Renderer> m_rendererModule;
    std::shared_ptr<Window> m_windowModule;
};

}  // namespace beet
