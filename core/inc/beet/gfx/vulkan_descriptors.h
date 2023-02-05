#pragma once

#include <beet/gfx/types.h>

#include <vulkan/vulkan.h>

namespace beet {
class Renderer;
}  // namespace beet

namespace beet::gfx {
class VulkanDescriptors {
   public:
    VulkanDescriptors(Renderer& renderer);
    ~VulkanDescriptors();

    VkDescriptorSetLayout& get_global_descriptor_set() { return m_globalSetLayout; }
    VkDescriptorSetLayout& get_texture_descriptor_set() { return m_textureLayout; }
    VkDescriptorPool get_descriptor_pool() { return m_descriptorPool; }

   private:
    void init_descriptors();

   private:
    VkDescriptorSetLayout m_globalSetLayout;
    VkDescriptorSetLayout m_textureLayout;
    VkDescriptorPool m_descriptorPool;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx
