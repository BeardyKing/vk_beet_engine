#include <beet/gfx/vulkan_rhi/vulkan_device.h>

#include <beet/engine.h>
#include <beet/log.h>

#include <VkBootstrap.h>

#include <iostream>

namespace beet::gfx {
VulkanDevice::VulkanDevice(Engine& engine) : m_engine(engine) {}

VulkanDevice::~VulkanDevice() {
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    for (auto& m_swapchainImageView : m_swapchainImageViews) {
        vkDestroyImageView(m_device, m_swapchainImageView, nullptr);
    }

    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
    vkDestroyInstance(m_instance, nullptr);
}

bool VulkanDevice::init() {
    init_vulkan();
    init_swapchain();

    return true;
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
    m_debug_messenger = vkb_inst.debug_messenger;

    m_engine.get_window_module().lock()->create_surface(m_instance, m_surface);

    vkb::PhysicalDeviceSelector selector{vkb_inst};
    vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 1).set_surface(m_surface).select().value();

    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    m_device = vkbDevice.device;
    m_chosenGPU = physicalDevice.physical_device;
}

void VulkanDevice::init_swapchain() {
    const vec2u extent = m_engine.get_window_module().lock()->get_window_size();

    vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface};
    vkb::Swapchain vkbSwapchain = swapchainBuilder.use_default_format_selection()
                                      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                      .set_desired_extent(extent.x, extent.y)
                                      .build()
                                      .value();

    m_swapchain = vkbSwapchain.swapchain;
    m_swapchainImages = vkbSwapchain.get_images().value();
    m_swapchainImageViews = vkbSwapchain.get_image_views().value();
    m_swapchainImageFormat = vkbSwapchain.image_format;
}

}  // namespace beet::gfx