#include <beet/gfx/vulkan_device.h>
#include <beet/gfx/vulkan_initializers.h>
#include <beet/gfx/vulkan_swapchain.h>

#include <VkBootstrap.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanSwapchain::VulkanSwapchain(Renderer& renderer) : m_renderer(renderer) {
    init_swapchain();
}
VulkanSwapchain::~VulkanSwapchain() {
    auto device = m_renderer.get_device();

    vkDestroySwapchainKHR(device, m_swapchain, nullptr);

    for (auto& m_swapchainImageView : m_swapchainImageViews) {
        vkDestroyImageView(device, m_swapchainImageView, nullptr);
    }

    log::debug("VulkanSwapchain destroyed");
}

void VulkanSwapchain::init_swapchain() {
    const vec2u extent = m_renderer.get_engine().get_window_module().lock()->get_window_size();
    auto device = m_renderer.get_device();
    auto physicalDevice = m_renderer.get_physical_device();
    auto surface = m_renderer.get_surface();

    vkb::SwapchainBuilder swapchainBuilder{physicalDevice, device, surface};
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

void VulkanSwapchain::acquire_next_image() {
    auto device = m_renderer.get_device();
    auto semaphore = m_renderer.get_present_semaphore();

    auto result = vkAcquireNextImageKHR(device, m_swapchain, 1000000000, semaphore, nullptr, &m_swapchainIndex);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to get next swapchain image");
}

void VulkanSwapchain::present() {
    auto graphicsQueue = m_renderer.get_graphics_queue();
    auto renderSemaphore = m_renderer.get_render_semaphore();

    VkPresentInfoKHR presentInfo = init::present_info();
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pImageIndices = &m_swapchainIndex;

    auto result = vkQueuePresentKHR(graphicsQueue, &presentInfo);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to present image");
}

}  // namespace beet::gfx
