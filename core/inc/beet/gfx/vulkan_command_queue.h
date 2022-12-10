#pragma once

#include <vulkan/vulkan.h>

#include <vector>
namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {

class VulkanCommandQueue {
   public:
    VulkanCommandQueue(Renderer& renderer);
    ~VulkanCommandQueue();

    void add_command(VkCommandBuffer cmd);
    void submit();

   private:
    std::vector<VkCommandBuffer> m_commandQueue;

   private:
    Renderer& m_renderer;
};
}  // namespace beet::gfx