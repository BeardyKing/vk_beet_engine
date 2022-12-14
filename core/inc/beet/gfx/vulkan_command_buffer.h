#pragma once

#include <beet/gfx/types.h>

#include <vulkan/vulkan.h>
#include <array>

namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {

class VulkanCommandBuffer {
   public:
    VulkanCommandBuffer(Renderer& renderer);
    ~VulkanCommandBuffer();

    void reset();
    void recreate();

    void begin_recording();
    void end_recording();
    void next_frame() { m_currentFrame = ++m_currentFrame % FRAME_OVERLAP_COUNT; }

    VkCommandBuffer& get_main_command_buffer() { return m_frames[m_currentFrame].mainCommandBuffer; }
    VkSemaphore& get_present_semaphore() { return m_frames[m_currentFrame].presentSemaphore; }
    VkSemaphore& get_render_semaphore() { return m_frames[m_currentFrame].renderSemaphore; }
    VkFence& get_render_fence() { return m_frames[m_currentFrame].renderFence; }
    AllocatedBuffer& get_camera_buffer() { return m_frames[m_currentFrame].cameraBuffer; }
    VkDescriptorSet& get_global_descriptor() { return m_frames[m_currentFrame].globalDescriptor; }

    std::array<FrameData, FRAME_OVERLAP_COUNT>& get_frames() { return m_frames; }

   private:
    void init_commands();
    void cleanup();

   private:
    uint32_t m_currentFrame{0};
    std::array<FrameData, FRAME_OVERLAP_COUNT> m_frames;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
