#include <beet/gfx/vulkan_descriptors.h>
#include <beet/gfx/vulkan_initializers.h>

#include <beet/assert.h>
#include <beet/log.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanDescriptors::VulkanDescriptors(Renderer& renderer) : m_renderer(renderer) {
    init_descriptors();
}

VulkanDescriptors::~VulkanDescriptors() {
    auto device = m_renderer.get_vulkan_device()->get_device();

    // TODO: managed getting descriptors via enum instead of having individual functions.
    vkDestroyDescriptorSetLayout(device, m_textureLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, m_globalSetLayout, nullptr);
    vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);

    log::debug("VulkanDescriptors destroyed");
}

void VulkanDescriptors::init_descriptors() {
    auto device = m_renderer.get_vulkan_device()->get_device();

    {
        std::vector<VkDescriptorPoolSize> sizes = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
                                                   {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}};

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = 10;
        pool_info.poolSizeCount = (uint32_t)sizes.size();
        pool_info.pPoolSizes = sizes.data();

        auto result = vkCreateDescriptorPool(device, &pool_info, nullptr, &m_descriptorPool);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to create descriptor pool");
    }
    {
        VkDescriptorSetLayoutBinding camBufferBinding = {};
        camBufferBinding.binding = 0;
        camBufferBinding.descriptorCount = 1;
        camBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        camBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo setInfo = {};
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setInfo.pNext = nullptr;
        setInfo.bindingCount = 1;
        setInfo.flags = 0;
        setInfo.pBindings = &camBufferBinding;

        auto resultLayout = vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &m_globalSetLayout);
        BEET_ASSERT_MESSAGE(resultLayout == VK_SUCCESS, "Error: Vulkan failed to create descriptor set layout");
    }
    {
        VkDescriptorSetLayoutBinding textureBind = init::descriptorset_layout_binding(
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

        VkDescriptorSetLayoutCreateInfo setInfo = {};
        setInfo.bindingCount = 1;
        setInfo.flags = 0;
        setInfo.pNext = nullptr;
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setInfo.pBindings = &textureBind;

        auto resultLayout = vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &m_textureLayout);
        BEET_ASSERT_MESSAGE(resultLayout == VK_SUCCESS, "Error: Vulkan failed to create descriptor set layout texture");
    }
    {
        for (auto& frame : m_renderer.get_vulkan_command_buffer()->get_frame_data()) {
            frame.cameraBuffer = m_renderer.get_vulkan_buffer()->create_buffer(
                sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.pNext = nullptr;
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_descriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &m_globalSetLayout;
            vkAllocateDescriptorSets(device, &allocInfo, &frame.globalDescriptor);

            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = frame.cameraBuffer.buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(GPUCameraData);

            VkWriteDescriptorSet setWrite = {};
            setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrite.pNext = nullptr;
            setWrite.dstBinding = 0;
            setWrite.dstSet = frame.globalDescriptor;
            setWrite.descriptorCount = 1;
            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            setWrite.pBufferInfo = &bufferInfo;
            vkUpdateDescriptorSets(device, 1, &setWrite, 0, nullptr);
        }
    }
}

}  // namespace beet::gfx
