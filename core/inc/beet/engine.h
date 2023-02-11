#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <beet/device.h>
#include <beet/input_manager.h>
#include <beet/renderer.h>
#include <beet/resource_manager.h>
#include <beet/scene.h>
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
    bool is_client();

    inline void swap_frame();

   private:
    std::vector<std::shared_ptr<Subsystem>> m_engineModules;

    std::shared_ptr<Renderer> m_rendererModule;
    std::shared_ptr<Window> m_windowModule;
    std::shared_ptr<ResourceManager> m_resourceModule;
    std::shared_ptr<Scene> m_sceneModule;
    std::shared_ptr<Input> m_inputModule;
};

}  // namespace beet
