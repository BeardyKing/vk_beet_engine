#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/gfx/vulkan_render_pass.h>
#include <beet/log.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanRenderPass::VulkanRenderPass(Renderer& renderer) : m_renderer(renderer) {
    init_default_renderpass();
    init_framebuffers();
}

VulkanRenderPass::~VulkanRenderPass() {
    auto device = m_renderer.get_device();

    vkDestroyRenderPass(device, m_renderPass, nullptr);

    for (int i = 0; i < m_framebuffers.size(); i++) {
        vkDestroyFramebuffer(device, m_framebuffers[i], nullptr);
    }

    log::debug("VulkanRenderPass destroyed");
}

void VulkanRenderPass::init_default_renderpass() {
    auto imageFormat = m_renderer.get_swapchain_image_format();
    auto device = m_renderer.get_device();

    VkAttachmentDescription color_attachment = {};
    color_attachment.format = imageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    auto result = vkCreateRenderPass(device, &render_pass_info, nullptr, &m_renderPass);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to setup render pass");
}

void VulkanRenderPass::init_framebuffers() {
    const vec2u extent = m_renderer.get_engine().get_window_module().lock()->get_window_size();
    auto device = m_renderer.get_device();
    auto swapchainImages = m_renderer.get_swapchain_images();
    auto swapchainImageViews = m_renderer.get_swapchain_image_views();

    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = m_renderPass;
    fb_info.attachmentCount = 1;
    fb_info.width = extent.x;
    fb_info.height = extent.y;
    fb_info.layers = 1;

    const uint32_t swapchainImageCount = swapchainImages.size();
    m_framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

    for (int i = 0; i < swapchainImageCount; i++) {
        fb_info.pAttachments = &swapchainImageViews[i];
        auto result = vkCreateFramebuffer(device, &fb_info, nullptr, &m_framebuffers[i]);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed create framebuffers");
    }
}

}  // namespace beet::gfx