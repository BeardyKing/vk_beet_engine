#pragma once

#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_swapchain.h>

#include <beet/asset_loader.h>
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

    Engine& get_engine() { return m_engine; }
    VkDevice get_device() { return m_device->get_device(); }
    VkPhysicalDevice get_physical_device() { return m_device->get_physical_device(); }
    VkSurfaceKHR get_surface() { return m_device->get_surface(); }

   private:
    Engine& m_engine;

   private:
    std::shared_ptr<gfx::VulkanDevice> m_device;
    std::shared_ptr<gfx::VulkanSwapchain> m_swapchain;
};

}  // namespace beet
