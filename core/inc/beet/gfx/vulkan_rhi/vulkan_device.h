#pragma once
#include <beet/gfx/i_device.h>

#include <vulkan/vulkan.h>
namespace beet {
class Engine;
}  // namespace beet

namespace beet::gfx {
class VulkanDevice : public IDevice {
   public:
    VulkanDevice() = default;
    explicit VulkanDevice(Engine& engine);
    virtual bool init() override;

   private:
    Engine* m_engine;
};
}  // namespace beet::gfx
