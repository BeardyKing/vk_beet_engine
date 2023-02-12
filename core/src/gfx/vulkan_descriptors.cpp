#include <beet/gfx/vulkan_descriptors.h>

#include <beet/gfx/vulkan_command_buffer.h>
#include <beet/gfx/vulkan_device.h>
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

    //===DESCRIPTOR POOLS===//
    {
        std::vector<VkDescriptorPoolSize> sizes = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
                                                   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
                                                   {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}};

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = 0;
        poolInfo.maxSets = 10;
        poolInfo.poolSizeCount = (uint32_t)sizes.size();
        poolInfo.pPoolSizes = sizes.data();

        auto result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to create descriptor pool");
    }
    //===GLOBAL BINDINGS====//
    {
        auto cameraBind =
            init::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);

        auto sceneBind = init::descriptorset_layout_binding(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);

        std::array<VkDescriptorSetLayoutBinding, 2> globalBindings = {cameraBind, sceneBind};

        VkDescriptorSetLayoutCreateInfo setInfo = {};
        setInfo.bindingCount = (uint32_t)globalBindings.size();
        setInfo.flags = 0;
        setInfo.pNext = nullptr;
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setInfo.pBindings = globalBindings.data();

        vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &m_globalSetLayout);
    }
    //===MATERIAL BINDINGS===//
    {
        auto albedoBind = init::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                             VK_SHADER_STAGE_FRAGMENT_BIT, 0);
        auto normalBind = init::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                             VK_SHADER_STAGE_FRAGMENT_BIT, 1);
        auto metallicBind = init::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                               VK_SHADER_STAGE_FRAGMENT_BIT, 2);
        auto roughnessBind = init::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                VK_SHADER_STAGE_FRAGMENT_BIT, 3);
        auto occlusionBind = init::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                VK_SHADER_STAGE_FRAGMENT_BIT, 4);

        std::array<VkDescriptorSetLayoutBinding, 5> textureBindings{
            albedoBind, normalBind, metallicBind, roughnessBind, occlusionBind,
        };

        VkDescriptorSetLayoutCreateInfo setInfo = {};
        setInfo.bindingCount = (uint32_t)textureBindings.size();
        setInfo.flags = 0;
        setInfo.pNext = nullptr;
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setInfo.pBindings = textureBindings.data();

        auto resultLayout = vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &m_textureLayout);
        BEET_ASSERT_MESSAGE(resultLayout == VK_SUCCESS, "Error: Vulkan failed to create descriptor set layout texture");
    }
    {
        const size_t sceneParamBufferSize =
            FRAME_OVERLAP_COUNT * Renderer::pad_uniform_buffer_size(sizeof(GPUSceneData), "GPUSceneData");
        m_renderer.get_vulkan_command_buffer()->get_scene_data_buffer() = m_renderer.get_vulkan_buffer()->create_buffer(
            sceneParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
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

            VkDescriptorBufferInfo cameraInfo;
            cameraInfo.buffer = frame.cameraBuffer.buffer;
            cameraInfo.offset = 0;
            cameraInfo.range = sizeof(GPUCameraData);

            VkDescriptorBufferInfo sceneInfo;
            sceneInfo.buffer = m_renderer.get_vulkan_command_buffer()->get_scene_data_buffer().buffer;
            sceneInfo.offset = 0;
            sceneInfo.range = sizeof(GPUSceneData);

            VkWriteDescriptorSet cameraWrite = init::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                                             frame.globalDescriptor, &cameraInfo, 0);

            VkWriteDescriptorSet sceneWrite = init::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                                                                            frame.globalDescriptor, &sceneInfo, 1);

            VkWriteDescriptorSet setWrites[] = {cameraWrite, sceneWrite};

            vkUpdateDescriptorSets(device, 2, setWrites, 0, nullptr);
        }
    }
}

}  // namespace beet::gfx
