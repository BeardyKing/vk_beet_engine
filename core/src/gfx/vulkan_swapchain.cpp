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
    cleanup();
    log::debug("VulkanSwapchain destroyed");
}

void VulkanSwapchain::cleanup() {
    auto device = m_renderer.get_device();
    auto allocator = m_renderer.get_allocator();

    vkDestroySwapchainKHR(device, m_swapchain, nullptr);

    vkDestroyImageView(device, m_depthImageView, nullptr);
    vmaDestroyImage(allocator, m_depthImage.image, m_depthImage.allocation);

    for (auto& m_swapchainImageView : m_swapchainImageViews) {
        vkDestroyImageView(device, m_swapchainImageView, nullptr);
    }
}

void VulkanSwapchain::recreate() {
    cleanup();
    init_swapchain();
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

    VkExtent3D depthImageExtent = {extent.x, extent.y, 1};
    m_depthFormat = VK_FORMAT_D32_SFLOAT;

    VkImageCreateInfo depthImageInfo =
        init::image_create_info(m_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

    VmaAllocationCreateInfo depthImageAllocInfo = {};
    depthImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    depthImageAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    auto allocator = m_renderer.get_allocator();
    auto resultAlloc = vmaCreateImage(allocator, &depthImageInfo, &depthImageAllocInfo, &m_depthImage.image,
                                      &m_depthImage.allocation, nullptr);

    BEET_ASSERT_MESSAGE(resultAlloc == VK_SUCCESS, "Error: Vulkan - VMA failed to alloc depth image");
    VkImageViewCreateInfo depthImageViewInfo =
        init::imageview_create_info(m_depthFormat, m_depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

    auto result = vkCreateImageView(device, &depthImageViewInfo, nullptr, &m_depthImageView);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to create depth image view");
}

VkResult VulkanSwapchain::acquire_next_image() {
    auto device = m_renderer.get_device();
    auto semaphore = m_renderer.get_present_semaphore();

    auto result = vkAcquireNextImageKHR(device, m_swapchain, 1000000000, semaphore, nullptr, &m_swapchainIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return result;
    }
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to get next swapchain image");
    return result;
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
    //    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to present image");
}

}  // namespace beet::gfx
