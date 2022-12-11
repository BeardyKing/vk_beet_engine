#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/gfx/vulkan_initializers.h>
#include <beet/gfx/vulkan_render_pass.h>
#include <beet/log.h>
#include <beet/renderer.h>

namespace beet::gfx {

VulkanRenderPass::VulkanRenderPass(Renderer& renderer) : m_renderer(renderer) {
    init_default_renderpass();
    init_framebuffers();
    init_sync_structures();
}

VulkanRenderPass::~VulkanRenderPass() {
    cleanup();
    log::debug("VulkanRenderPass destroyed");
}

void VulkanRenderPass::cleanup() {
    auto device = m_renderer.get_device();
    vkDeviceWaitIdle(device);
    vkDestroyRenderPass(device, m_renderPass, nullptr);

    for (int i = 0; i < m_framebuffers.size(); i++) {
        vkDestroyFramebuffer(device, m_framebuffers[i], nullptr);
    }
}

void VulkanRenderPass::recreate() {
    cleanup();

    init_default_renderpass();
    init_framebuffers();
    init_sync_structures();
}

void VulkanRenderPass::init_default_renderpass() {
    auto imageFormat = m_renderer.get_swapchain_image_format();
    auto device = m_renderer.get_device();
    auto depthFormat = m_renderer.get_depth_format();

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

    VkAttachmentDescription depth_attachment = {};
    depth_attachment.flags = 0;
    depth_attachment.format = depthFormat;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depth_dependency = {};
    depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depth_dependency.dstSubpass = 0;
    depth_dependency.srcStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.srcAccessMask = 0;
    depth_dependency.dstStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dependencies[2] = {dependency, depth_dependency};
    VkAttachmentDescription attachments[2] = {color_attachment, depth_attachment};

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = &attachments[0];
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = &dependencies[0];

    auto result = vkCreateRenderPass(device, &render_pass_info, nullptr, &m_renderPass);
    BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed to setup render pass");
}

void VulkanRenderPass::init_framebuffers() {
    const vec2u extent = m_renderer.get_engine().get_window_module().lock()->get_window_size();
    auto device = m_renderer.get_device();
    auto swapchainImages = m_renderer.get_swapchain_images();
    auto swapchainImageViews = m_renderer.get_swapchain_image_views();
    auto depthImageView = m_renderer.get_depth_image_view();

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
        VkImageView attachments[2];
        attachments[0] = swapchainImageViews[i];
        attachments[1] = depthImageView;

        fb_info.pAttachments = attachments;
        fb_info.attachmentCount = 2;

        auto result = vkCreateFramebuffer(device, &fb_info, nullptr, &m_framebuffers[i]);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed create framebuffers");
    }
}
void VulkanRenderPass::init_sync_structures() {
    auto device = m_renderer.get_device();

    VkFenceCreateInfo fenceCreateInfo = init::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphoreCreateInfo = init::semaphore_create_info(0);

    for (auto& frame : m_renderer.get_frame_data()) {
        {
            auto result = vkCreateFence(device, &fenceCreateInfo, nullptr, &frame.renderFence);
            BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed create fence");
        }
        {
            auto result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.presentSemaphore);
            BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed create present semaphore");
        }
        {
            auto result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.renderSemaphore);
            BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed create render semaphore");
        }
    }

    //    VkFenceCreateInfo fenceCreateInfo = {};
    //    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //    fenceCreateInfo.pNext = nullptr;
    //    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    //    {
    //        auto result = vkCreateFence(device, &fenceCreateInfo, nullptr, &m_renderFence);
    //    }
    //
    //    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    //    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    //    semaphoreCreateInfo.pNext = nullptr;
    //    semaphoreCreateInfo.flags = 0;
    //    {
    //        auto result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore);
    //    }
    //    {
    //        auto result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore);
    //    }
}
void VulkanRenderPass::sync() {
    auto device = m_renderer.get_device();
    auto& renderFence = m_renderer.get_render_fence();

    {
        auto result = vkWaitForFences(device, 1, &renderFence, true, 1000000000);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed wait for render fence");
    }
    {
        auto result = vkResetFences(device, 1, &renderFence);
        BEET_ASSERT_MESSAGE(result == VK_SUCCESS, "Error: Vulkan failed reset render fence");
    }
}
VkRenderPassBeginInfo VulkanRenderPass::create_begin_info() {
    auto idx = m_renderer.get_swapchain_index();
    const vec2u size = m_renderer.get_engine().get_window_module().lock()->get_window_size();
    VkExtent2D extent{size.x, size.y};

    auto info = init::renderpass_begin_info(m_renderPass, extent, m_framebuffers[idx]);

    return info;
}

}  // namespace beet::gfx