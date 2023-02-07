#include <beet/gfx/vulkan_device.h>

#include <beet/gfx/vulkan_initializers.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/renderer.h>

#include <VkBootstrap.h>

#include <iostream>

namespace beet::gfx {
VulkanDevice::VulkanDevice(Renderer& renderer) : m_renderer(renderer) {
    init_vulkan();
}

VulkanDevice::~VulkanDevice() {
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
    vkDestroyInstance(m_instance, nullptr);
    log::debug("VulkanDevice destroyed");
}

void VulkanDevice::init_vulkan() {
    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("Beet Engine")
                        .request_validation_layers(true)
                        .require_api_version(1, 1, 0)
                        .use_default_debug_messenger()  // TODO setup callback to work with stb log
                        .build();

    vkb::Instance vkb_inst = inst_ret.value();
    m_instance = vkb_inst.instance;
    m_debugMessenger = vkb_inst.debug_messenger;

    Window::create_surface(m_instance, m_surface);

    vkb::PhysicalDeviceSelector selector{vkb_inst};
    vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 1).set_surface(m_surface).select().value();

    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    m_device = vkbDevice.device;
    m_chosenGPU = physicalDevice.physical_device;

    m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

}  // namespace beet::gfx