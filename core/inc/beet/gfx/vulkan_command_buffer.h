#pragma once

#include <vulkan/vulkan.h>

namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {

class VulkanCommandBuffer {
   public:
    VulkanCommandBuffer(Renderer& renderer);
    ~VulkanCommandBuffer();

    void reset();

    void begin_recording();
    void end_recording();

    VkCommandBuffer get_main_command_buffer() { return m_mainCommandBuffer; };

   private:
    void init_commands();

   private:
    VkCommandPool m_commandPool;
    VkCommandBuffer m_mainCommandBuffer;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
