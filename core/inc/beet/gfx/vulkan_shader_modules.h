#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

#include <beet/types.h>

namespace beet {
class Renderer;

struct ShaderModules {
    VkShaderModule shader;
    gfx::ShaderType type;
};

}  // namespace beet

namespace beet::gfx {

class VulkanShaderModules {
   public:
    VulkanShaderModules(Renderer& renderer);
    ~VulkanShaderModules();

    void load(const std::vector<char>& src, ShaderType shaderType);
    VkShaderModule get_module(ShaderType type);

   private:
    VkShaderModule create_shader_module(const std::vector<char>& src);

   private:
    std::array<ShaderModules, (size_t)ShaderType::LAST> m_shaderModules;

   private:
    Renderer& m_renderer;
};

}  // namespace beet::gfx