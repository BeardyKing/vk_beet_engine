#include <beet/gfx/vulkan_shader_modules.h>

#include <beet/assert.h>
#include <beet/log.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanShaderModules::VulkanShaderModules(Renderer& renderer) : m_renderer(renderer) {}

VulkanShaderModules::~VulkanShaderModules() {
    auto device = m_renderer.get_vulkan_device()->get_device();

    for (auto& module : m_shaderModules) {
        if (module.shader != VK_NULL_HANDLE) {
            vkDestroyShaderModule(device, module.shader, nullptr);
        }
    }
}

VkShaderModule VulkanShaderModules::get_module(ShaderType shaderType) {
    BEET_ASSERT_MESSAGE(!((size_t)shaderType >= (size_t)ShaderType::LAST), "Invalid Shader type: {}", (int)shaderType)
    return m_shaderModules[(int)shaderType].shader;
}

void VulkanShaderModules::load(const std::vector<char>& src, ShaderType shaderType) {
    BEET_ASSERT_MESSAGE(!((size_t)shaderType >= (size_t)ShaderType::LAST), "Invalid Shader type: {}", (int)shaderType)
    m_shaderModules[(int)shaderType] = ShaderModules{create_shader_module(src), shaderType};
}

VkShaderModule VulkanShaderModules::create_shader_module(const std::vector<char>& src) {
    auto device = m_renderer.get_vulkan_device()->get_device();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = src.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "failed to create shader module!")

    return shaderModule;
}

}  // namespace beet::gfx