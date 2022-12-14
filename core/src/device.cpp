//#include <beet/device.h>
//
//#include <beet/assert.h>
//#include <beet/asset_loader.h>
//#include <beet/engine.h>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//
//#include <algorithm>
//#include <limits>
//#include <set>
//#include <unordered_map>
//
//#include <assimp/postprocess.h>
//#include <assimp/scene.h>
//#include <assimp/Importer.hpp>
//
// namespace beet {
//
// Device::Device(beet::Engine& engine) : m_engine(engine) {}
// Device::~Device() {}
//
// void Device::on_awake() {
//    create_instance();
//    setup_debug_messenger();
//    create_surface();
//    pick_physical_device();
//    create_logical_device();
//    create_swap_chain();
//    create_image_views();
//    create_render_pass();
//    create_descriptor_set_layout();
//    create_graphics_pipeline();
//    create_command_pool();
//    create_depth_resources();
//    create_framebuffers();
//    create_texture_image();
//    create_texture_image_view();
//    load_model();
//    create_texture_sampler();
//    create_vertex_buffer();
//    create_index_buffer();
//    create_uniform_buffers();
//    create_descriptor_pool();
//    create_descriptor_sets();
//    create_command_buffers();
//    create_sync_objects();
//}
//
// void Device::on_update(double deltaTime) {
//    draw(deltaTime);
//}
//
// void Device::on_late_update() {}
//
// void Device::on_destroy() {
//    vkDeviceWaitIdle(m_device);
//
//    vkDestroyImageView(m_device, m_depthImageView, nullptr);
//    vkDestroyImage(m_device, m_depthImage, nullptr);
//    vkFreeMemory(m_device, m_depthImageMemory, nullptr);
//
//    cleanup_swap_chain();
//
//    vkDestroySampler(m_device, m_textureSampler, nullptr);
//    vkDestroyImageView(m_device, m_textureImageView, nullptr);
//
//    vkDestroyImage(m_device, m_textureImage, nullptr);
//    vkFreeMemory(m_device, m_textureImageMemory, nullptr);
//
//    vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
//    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
//    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        vkDestroyBuffer(m_device, m_uniformBuffers[i], nullptr);
//        vkFreeMemory(m_device, m_uniformBuffersMemory[i], nullptr);
//    }
//
//    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
//    vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
//
//    vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
//    vkFreeMemory(m_device, m_indexBufferMemory, nullptr);
//
//    vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
//    vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
//        vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
//        vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
//    }
//
//    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
//
//    vkDestroyDevice(m_device, nullptr);
//
//    if (ENABLED_VALIDATION_LAYERS) {
//        destroy_debug_utils_messengerEXT(m_instance, m_debugMessenger, nullptr);
//    }
//
//    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
//    vkDestroyInstance(m_instance, nullptr);
//
//    log::debug("Device destroyed");
//}
//
// void Device::recreate_swap_chain() {
//    auto window = m_engine.get_window_module().lock();
//    vec2i size{0};
//    while (!size.x || !size.y) {
//        window->get_framebuffer_size(size);
//        window->wait_events();
//    }
//
//    vkDeviceWaitIdle(m_device);
//
//    cleanup_swap_chain();
//
//    create_swap_chain();
//    create_image_views();
//    create_depth_resources();
//    create_framebuffers();
//}
//
// void Device::cleanup_swap_chain() {
//    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
//        vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
//    }
//
//    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
//        vkDestroyImageView(m_device, m_swapChainImageViews[i], nullptr);
//    }
//
//    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
//}
//
// void Device::draw(double deltaTime) {
//    vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
//    vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
//
//    uint32_t imageIndex;
//    VkResult swapChainStatus = vkAcquireNextImageKHR(
//        m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
//
//    // TODO MOVE THIS TO ON RESIZE
//    if (swapChainStatus == VK_ERROR_OUT_OF_DATE_KHR) {
//        recreate_swap_chain();
//        return;
//    } else if (swapChainStatus != VK_SUCCESS && swapChainStatus != VK_SUBOPTIMAL_KHR) {
//        BEET_ASSERT_MESSAGE(BEET_FALSE, "failed to acquire swap chain image!");
//    }
//
//    update_uniform_buffer(m_currentFrame, deltaTime);
//
//    vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
//
//    vkResetCommandBuffer(m_commandBuffers[m_currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
//    record_command_buffer(m_commandBuffers[m_currentFrame], imageIndex);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
//    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
//    submitInfo.waitSemaphoreCount = 1;
//    submitInfo.pWaitSemaphores = waitSemaphores;
//    submitInfo.pWaitDstStageMask = waitStages;
//
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
//
//    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
//    submitInfo.signalSemaphoreCount = 1;
//    submitInfo.pSignalSemaphores = signalSemaphores;
//
//    VkResult sumbitQueueResult = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]);
//    BEET_ASSERT_MESSAGE(sumbitQueueResult == VK_SUCCESS, "failed to submit draw command buffer!");
//
//    VkPresentInfoKHR presentInfo{};
//    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//    presentInfo.waitSemaphoreCount = 1;
//    presentInfo.pWaitSemaphores = signalSemaphores;
//
//    VkSwapchainKHR swapChains[] = {m_swapChain};
//    presentInfo.swapchainCount = 1;
//    presentInfo.pSwapchains = swapChains;
//
//    presentInfo.pImageIndices = &imageIndex;
//
//    VkResult presentResult = vkQueuePresentKHR(m_presentQueue, &presentInfo);
//
//    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
//        m_framebufferResized = false;
//        recreate_swap_chain();
//    } else if (presentResult != VK_SUCCESS) {
//        BEET_ASSERT_MESSAGE(BEET_FALSE, "failed to present swap chain image!");
//    }
//
//    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//}
//
// void Device::update_uniform_buffer(uint32_t currentFrame, double deltaTime) {
//    static float timePassed{};
//    timePassed += (float)deltaTime * 0.25f;
//
//    UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), timePassed * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.proj =
//        glm::perspective(glm::radians(45.0f), m_swapChainExtent.width / (float)m_swapChainExtent.height, 0.1f, 10.0f);
//    ubo.proj[1][1] *= -1;
//
//    void* data;
//    vkMapMemory(m_device, m_uniformBuffersMemory[currentFrame], 0, sizeof(ubo), 0, &data);
//    memcpy(data, &ubo, sizeof(ubo));
//    vkUnmapMemory(m_device, m_uniformBuffersMemory[currentFrame]);
//}
//
// void Device::create_vertex_buffer() {
//    VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
//                  stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
//    memcpy(data, m_vertices.data(), (size_t)bufferSize);
//    vkUnmapMemory(m_device, stagingBufferMemory);
//
//    create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);
//
//    copy_buffer(stagingBuffer, m_vertexBuffer, bufferSize);
//
//    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
//    vkFreeMemory(m_device, stagingBufferMemory, nullptr);
//}
//
// void Device::create_index_buffer() {
//    VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
//                  stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
//    memcpy(data, m_indices.data(), (size_t)bufferSize);
//    vkUnmapMemory(m_device, stagingBufferMemory);
//
//    create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);
//
//    copy_buffer(stagingBuffer, m_indexBuffer, bufferSize);
//
//    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
//    vkFreeMemory(m_device, stagingBufferMemory, nullptr);
//}
//
// void Device::create_uniform_buffers() {
//    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//
//    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//    m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        create_buffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i],
//                      m_uniformBuffersMemory[i]);
//    }
//}
//
// void Device::create_descriptor_pool() {
//    std::array<VkDescriptorPoolSize, 2> poolSizes{};
//    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//
//    VkDescriptorPoolCreateInfo poolInfo{};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//    poolInfo.pPoolSizes = poolSizes.data();
//    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//
//    VkResult descriptorPoolResult = vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);
//    BEET_ASSERT_MESSAGE(descriptorPoolResult == VK_SUCCESS, "failed to create descriptor pool!")
//}
//
// void Device::create_descriptor_sets() {
//    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
//    VkDescriptorSetAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    allocInfo.descriptorPool = m_descriptorPool;
//    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//    allocInfo.pSetLayouts = layouts.data();
//
//    m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
//
//    VkResult descriptorSetResult = vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data());
//    BEET_ASSERT_MESSAGE(descriptorSetResult == VK_SUCCESS, "failed to create descriptor pool!")
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        VkDescriptorBufferInfo bufferInfo{};
//        bufferInfo.buffer = m_uniformBuffers[i];
//        bufferInfo.offset = 0;
//        bufferInfo.range = sizeof(UniformBufferObject);
//
//        VkDescriptorImageInfo imageInfo{};
//        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        imageInfo.imageView = m_textureImageView;
//        imageInfo.sampler = m_textureSampler;
//
//        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
//
//        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[0].dstSet = m_descriptorSets[i];
//        descriptorWrites[0].dstBinding = 0;
//        descriptorWrites[0].dstArrayElement = 0;
//        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[0].descriptorCount = 1;
//        descriptorWrites[0].pBufferInfo = &bufferInfo;
//
//        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[1].dstSet = m_descriptorSets[i];
//        descriptorWrites[1].dstBinding = 1;
//        descriptorWrites[1].dstArrayElement = 0;
//        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//        descriptorWrites[1].descriptorCount = 1;
//        descriptorWrites[1].pImageInfo = &imageInfo;
//
//        vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0,
//                               nullptr);
//    }
//}
//
// void Device::create_buffer(VkDeviceSize size,
//                           VkBufferUsageFlags usage,
//                           VkMemoryPropertyFlags properties,
//                           VkBuffer& buffer,
//                           VkDeviceMemory& bufferMemory) {
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size = size;
//    bufferInfo.usage = usage;
//    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    VkResult bufferResult = vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer);
//    BEET_ASSERT_MESSAGE(bufferResult == VK_SUCCESS, "failed to create vertex buffer!");
//
//    VkMemoryRequirements memRequirements;
//    vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocInfo.allocationSize = memRequirements.size;
//    allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);
//
//    VkResult allocationResult = vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory);
//    BEET_ASSERT_MESSAGE(allocationResult == VK_SUCCESS, "failed to allocate vertex buffer memory!")
//
//    vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
//}
//
// void Device::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
//    VkCommandBuffer commandBuffer = begin_single_time_commands();
//
//    VkBufferCopy copyRegion{};
//    copyRegion.size = size;
//    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//    end_single_time_commands(commandBuffer);
//}
//
// void Device::copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
//    VkCommandBuffer commandBuffer = begin_single_time_commands();
//
//    VkBufferImageCopy region{};
//    region.bufferOffset = 0;
//    region.bufferRowLength = 0;
//    region.bufferImageHeight = 0;
//
//    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    region.imageSubresource.mipLevel = 0;
//    region.imageSubresource.baseArrayLayer = 0;
//    region.imageSubresource.layerCount = 1;
//
//    region.imageOffset = {0, 0, 0};
//    region.imageExtent = {width, height, 1};
//
//    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//    end_single_time_commands(commandBuffer);
//}
//
// uint32_t Device::find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//            return i;
//        }
//    }
//
//    BEET_ASSERT_MESSAGE(BEET_FALSE, "failed to find suitable memory type!");
//}
//
// void Device::create_sync_objects() {
//    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
//
//    VkSemaphoreCreateInfo semaphoreInfo{};
//    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//    VkFenceCreateInfo fenceInfo{};
//    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
//        VkResult imageResult = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
//        VkResult renderResult = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
//        VkResult inflightResult = vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]);
//
//        BEET_ASSERT_MESSAGE(imageResult == VK_SUCCESS, "failed to create image available {} semaphore!", i);
//        BEET_ASSERT_MESSAGE(renderResult == VK_SUCCESS, "failed to create render finished {} semaphore!", i);
//        BEET_ASSERT_MESSAGE(inflightResult == VK_SUCCESS, "failed to create in flight {} fence!", i);
//    }
//}
//
// void Device::create_command_buffers() {
//    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.commandPool = m_commandPool;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();
//
//    VkResult commandBufferResult = vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data());
//    BEET_ASSERT_MESSAGE(commandBufferResult == VK_SUCCESS, "failed to allocate command buffers!");
//}
//
// void Device::record_command_buffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = 0;                   // Optional
//    beginInfo.pInheritanceInfo = nullptr;  // Optional
//
//    VkResult beginCommandBufferResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
//    BEET_ASSERT_MESSAGE(beginCommandBufferResult == VK_SUCCESS, "failed to begin recording command buffer!");
//
//    VkRenderPassBeginInfo renderPassInfo{};
//    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//    renderPassInfo.renderPass = m_renderPass;
//    renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
//    renderPassInfo.renderArea.offset = {0, 0};
//    renderPassInfo.renderArea.extent = m_swapChainExtent;
//
//    std::array<VkClearValue, 2> clearValues{};
//    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
//    clearValues[1].depthStencil = {1.0f, 0};
//
//    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//    renderPassInfo.pClearValues = clearValues.data();
//
//    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//    {
//        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
//
//        VkViewport viewport{};
//        viewport.x = 0.0f;
//        viewport.y = 0.0f;
//        viewport.width = static_cast<float>(m_swapChainExtent.width);
//        viewport.height = static_cast<float>(m_swapChainExtent.height);
//        viewport.minDepth = 0.0f;
//        viewport.maxDepth = 1.0f;
//        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//
//        VkRect2D scissor{};
//        scissor.offset = {0, 0};
//        scissor.extent = m_swapChainExtent;
//        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//
//        VkBuffer vertexBuffers[] = {m_vertexBuffer};
//        VkDeviceSize offsets[] = {0};
//        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
//                                &m_descriptorSets[m_currentFrame], 0, nullptr);
//        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
//    }
//    vkCmdEndRenderPass(commandBuffer);
//
//    VkResult endCommandBufferResult = vkEndCommandBuffer(commandBuffer);
//    BEET_ASSERT_MESSAGE(endCommandBufferResult == VK_SUCCESS, "failed to record command buffer!");
//}
//
// void Device::create_command_pool() {
//    QueueFamilyIndices queueFamilyIndices = find_queue_families(m_physicalDevice);
//
//    VkCommandPoolCreateInfo poolInfo{};
//    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
//
//    VkResult commandQueueResult = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool);
//    BEET_ASSERT_MESSAGE(commandQueueResult == VK_SUCCESS, "failed to create command pool!");
//}
//
// void Device::create_depth_resources() {
//    VkFormat depthFormat = find_depth_format();
//
//    create_image(m_swapChainExtent.width, m_swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
//                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage,
//                 m_depthImageMemory);
//    m_depthImageView = create_image_view(m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
//
//    //    transition_image_layout(m_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
//    //                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
//}
//
// void Device::create_texture_image() {
//    int texWidth{};
//    int texHeight{};
//    int texChannels{};
//    stbi_uc* pixelData = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//    VkDeviceSize imageSize = texWidth * texHeight * 4;
//
//    BEET_ASSERT_MESSAGE(pixelData, "failed to load texture image!");
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//
//    create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
//                  stagingBufferMemory);
//    void* data;
//    vkMapMemory(m_device, stagingBufferMemory, 0, imageSize, 0, &data);
//    memcpy(data, pixelData, static_cast<size_t>(imageSize));
//    vkUnmapMemory(m_device, stagingBufferMemory);
//
//    stbi_image_free(pixelData);
//
//    create_image(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
//                 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//                 m_textureImage, m_textureImageMemory);
//
//    transition_image_layout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
//                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//
//    copy_buffer_to_image(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth),
//                         static_cast<uint32_t>(texHeight));
//
//    transition_image_layout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
//    vkFreeMemory(m_device, stagingBufferMemory, nullptr);
//}
//
// void Device::create_texture_image_view() {
//    m_textureImageView = create_image_view(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
//}
//
// void Device::create_texture_sampler() {
//    VkPhysicalDeviceProperties properties{};
//    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
//
//    VkSamplerCreateInfo samplerInfo{};
//    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//    samplerInfo.magFilter = VK_FILTER_LINEAR;
//    samplerInfo.minFilter = VK_FILTER_LINEAR;
//    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.anisotropyEnable = VK_TRUE;
//    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
//    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//    samplerInfo.unnormalizedCoordinates = VK_FALSE;
//    samplerInfo.compareEnable = VK_FALSE;
//    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//    samplerInfo.mipLodBias = 0.0f;
//    samplerInfo.minLod = 0.0f;
//    samplerInfo.maxLod = 0.0f;
//
//    VkResult samplerResult = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_textureSampler);
//    BEET_ASSERT_MESSAGE(samplerResult == VK_SUCCESS, "failed to create texture sampler!");
//}
//
// void Device::load_model() {
//    std::string fileName = MODEL_PATH;
//
//    Assimp::Importer importer;
//
//    const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
//                                                                   aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
//    if (!scene) {
//        log::error("Failed to load mesh : {}", fileName);
//        BEET_ASSERT(BEET_FALSE);
//    }
//
//    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
//
//    for (int x = 0; x < scene->mNumMeshes; ++x) {
//        const aiMesh* model = scene->mMeshes[x];
//        for (size_t i = 0; i < model->mNumVertices; i++) {
//            aiVector3D position;
//            aiVector3D normal;
//            aiVector3D textureCoordinates{0, 0, 0};  // note assimp mTextureCoords are stored as vec3
//            aiVector3D tangent{0, 0, 0};
//
//            normal = model->mNormals[i];
//            position = model->mVertices[i];
//
//            if (model->HasTextureCoords(0)) {
//                textureCoordinates = model->mTextureCoords[0][i];
//            }
//
//            if (model->HasTangentsAndBitangents()) {
//                tangent = model->mTangents[i];
//            }
//
//            Vertex vertex{};
//
//            vertex.pos = {position.x, position.y, position.z};
//            vertex.texCoord = {textureCoordinates.x, textureCoordinates.y};
//            vertex.color = {1.0f, 1.0f, 1.0f};
//
//            if (uniqueVertices.count(vertex) == 0) {
//                uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
//                m_vertices.push_back(vertex);
//            }
//
//            m_indices.push_back(uniqueVertices[vertex]);
//        }
//    }
//}
//
// VkImageView Device::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
//    VkImageViewCreateInfo viewInfo{};
//    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//    viewInfo.image = image;
//    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//    viewInfo.format = format;
//    viewInfo.subresourceRange.aspectMask = aspectFlags;
//    viewInfo.subresourceRange.baseMipLevel = 0;
//    viewInfo.subresourceRange.levelCount = 1;
//    viewInfo.subresourceRange.baseArrayLayer = 0;
//    viewInfo.subresourceRange.layerCount = 1;
//
//    VkImageView imageView;
//    VkResult viewResult = vkCreateImageView(m_device, &viewInfo, nullptr, &imageView);
//    BEET_ASSERT_MESSAGE(viewResult == VK_SUCCESS, "failed to create texture image view!");
//
//    return imageView;
//}
//
// VkFormat Device::find_supported_format(const std::vector<VkFormat>& candidates,
//                                       VkImageTiling tiling,
//                                       VkFormatFeatureFlags features) {
//    for (VkFormat format : candidates) {
//        VkFormatProperties props;
//        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);
//
//        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
//            return format;
//        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
//            return format;
//        }
//    }
//    BEET_ASSERT_MESSAGE(BEET_FALSE, "failed to find supported format!");
//}
//
// VkFormat Device::find_depth_format() {
//    return find_supported_format({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
//                                 VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
//}
// bool Device::has_stencil_component(VkFormat format) {
//    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
//}
//
// void Device::create_image(uint32_t width,
//                          uint32_t height,
//                          VkFormat format,
//                          VkImageTiling tiling,
//                          VkImageUsageFlags usage,
//                          VkMemoryPropertyFlags properties,
//                          VkImage& image,
//                          VkDeviceMemory& imageMemory) {
//    VkImageCreateInfo imageInfo{};
//    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//    imageInfo.imageType = VK_IMAGE_TYPE_2D;
//    imageInfo.extent.width = width;
//    imageInfo.extent.height = height;
//    imageInfo.extent.depth = 1;
//    imageInfo.mipLevels = 1;
//    imageInfo.arrayLayers = 1;
//    imageInfo.format = format;
//    imageInfo.tiling = tiling;
//    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    imageInfo.usage = usage;
//    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    VkResult imageResult = vkCreateImage(m_device, &imageInfo, nullptr, &image);
//    BEET_ASSERT_MESSAGE(imageResult == VK_SUCCESS, "failed to create image!");
//
//    VkMemoryRequirements memRequirements;
//    vkGetImageMemoryRequirements(m_device, image, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocInfo.allocationSize = memRequirements.size;
//    allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);
//
//    VkResult memoryResult = vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory);
//    BEET_ASSERT_MESSAGE(memoryResult == VK_SUCCESS, "failed to allocate image memory!");
//
//    vkBindImageMemory(m_device, image, imageMemory, 0);
//}
//
// VkCommandBuffer Device::begin_single_time_commands() {
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = m_commandPool;
//    allocInfo.commandBufferCount = 1;
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//    return commandBuffer;
//}
//
// void Device::transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout
// newLayout) {
//    VkCommandBuffer commandBuffer = begin_single_time_commands();
//
//    VkImageMemoryBarrier barrier{};
//    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//    barrier.oldLayout = oldLayout;
//    barrier.newLayout = newLayout;
//    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.image = image;
//    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    barrier.subresourceRange.baseMipLevel = 0;
//    barrier.subresourceRange.levelCount = 1;
//    barrier.subresourceRange.baseArrayLayer = 0;
//    barrier.subresourceRange.layerCount = 1;
//
//    VkPipelineStageFlags sourceStage{};
//    VkPipelineStageFlags destinationStage{};
//
//    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//        barrier.srcAccessMask = 0;
//        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
//               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//    } else {
//        BEET_ASSERT_MESSAGE(BEET_FALSE, "unsupported layout transition!");
//    }
//
//    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
//    end_single_time_commands(commandBuffer);
//}
//
// void Device::end_single_time_commands(VkCommandBuffer commandBuffer) {
//    vkEndCommandBuffer(commandBuffer);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &commandBuffer;
//
//    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//    vkQueueWaitIdle(m_graphicsQueue);
//
//    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
//}
//
// void Device::create_framebuffers() {
//    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());
//
//    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
//        std::array<VkImageView, 2> attachments = {m_swapChainImageViews[i], m_depthImageView};
//
//        VkFramebufferCreateInfo framebufferInfo{};
//        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//        framebufferInfo.renderPass = m_renderPass;
//        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//        framebufferInfo.pAttachments = attachments.data();
//        framebufferInfo.width = m_swapChainExtent.width;
//        framebufferInfo.height = m_swapChainExtent.height;
//        framebufferInfo.layers = 1;
//
//        VkResult framebufferResult =
//            vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]);
//        BEET_ASSERT_MESSAGE(framebufferResult == VK_SUCCESS, "failed to create framebuffer!");
//    }
//}
//
// void Device::create_render_pass() {
//    VkAttachmentDescription colorAttachment{};
//    colorAttachment.format = m_swapChainImageFormat;
//    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//    VkAttachmentDescription depthAttachment{};
//    depthAttachment.format = find_depth_format();
//    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//    VkAttachmentReference colorAttachmentRef{};
//    colorAttachmentRef.attachment = 0;
//    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//    VkAttachmentReference depthAttachmentRef{};
//    depthAttachmentRef.attachment = 1;
//    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//    VkSubpassDescription subpass{};
//    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//    subpass.colorAttachmentCount = 1;
//    subpass.pColorAttachments = &colorAttachmentRef;
//    subpass.pDepthStencilAttachment = &depthAttachmentRef;
//
//    VkSubpassDependency dependency{};
//    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//    dependency.dstSubpass = 0;
//    dependency.srcStageMask =
//        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//    dependency.srcAccessMask = 0;
//    dependency.dstStageMask =
//        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
//    VkRenderPassCreateInfo renderPassInfo{};
//    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//    renderPassInfo.pAttachments = attachments.data();
//    renderPassInfo.subpassCount = 1;
//    renderPassInfo.pSubpasses = &subpass;
//    renderPassInfo.dependencyCount = 1;
//    renderPassInfo.pDependencies = &dependency;
//
//    VkResult renderPassResult = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
//    BEET_ASSERT_MESSAGE(renderPassResult == VK_SUCCESS, "failed to create render pass!");
//};
//
// void Device::create_descriptor_set_layout() {
//    VkDescriptorSetLayoutBinding uboLayoutBinding{};
//    uboLayoutBinding.binding = 0;
//    uboLayoutBinding.descriptorCount = 1;
//    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    uboLayoutBinding.pImmutableSamplers = nullptr;
//    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
//    samplerLayoutBinding.binding = 1;
//    samplerLayoutBinding.descriptorCount = 1;
//    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    samplerLayoutBinding.pImmutableSamplers = nullptr;
//    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
//    VkDescriptorSetLayoutCreateInfo layoutInfo{};
//    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//    layoutInfo.pBindings = bindings.data();
//
//    VkResult descriptorResult = vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout);
//    BEET_ASSERT_MESSAGE(descriptorResult == VK_SUCCESS, "failed to create descriptor set layout!");
//}
//
// void Device::create_graphics_pipeline() {
//    std::vector<char> vertCode = AssetLoader::read_file("../res/shaders/simple_shader.vert.spv");
//    std::vector<char> fragCode = AssetLoader::read_file("../res/shaders/simple_shader.frag.spv");
//
//    VkShaderModule vertShaderModule = create_shader_module(vertCode);
//    VkShaderModule fragShaderModule = create_shader_module(fragCode);
//
//    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//    vertShaderStageInfo.module = vertShaderModule;
//    vertShaderStageInfo.pName = "main";
//
//    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//    fragShaderStageInfo.module = fragShaderModule;
//    fragShaderStageInfo.pName = "main";
//
//    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
//
//    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//    vertexInputInfo.vertexBindingDescriptionCount = 0;
//    vertexInputInfo.pVertexBindingDescriptions = nullptr;  // Optional
//    vertexInputInfo.vertexAttributeDescriptionCount = 0;
//    vertexInputInfo.pVertexAttributeDescriptions = nullptr;  // Optional
//
//    auto bindingDescription = Vertex::get_binding_description();
//    auto attributeDescriptions = Vertex::get_attribute_descriptions();
//
//    vertexInputInfo.vertexBindingDescriptionCount = 1;
//    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
//
//    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//    inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//    VkPipelineViewportStateCreateInfo viewportState{};
//    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//    viewportState.viewportCount = 1;
//    viewportState.scissorCount = 1;
//
//    VkPipelineRasterizationStateCreateInfo rasterizer{};
//    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//    rasterizer.depthClampEnable = VK_FALSE;
//    rasterizer.rasterizerDiscardEnable = VK_FALSE;
//    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//    rasterizer.lineWidth = 1.0f;
//    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//    rasterizer.depthBiasEnable = VK_FALSE;
//
//    VkPipelineMultisampleStateCreateInfo multisampling{};
//    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//    multisampling.sampleShadingEnable = VK_FALSE;
//    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//    VkPipelineDepthStencilStateCreateInfo depthStencil{};
//    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//    depthStencil.depthTestEnable = VK_TRUE;
//    depthStencil.depthWriteEnable = VK_TRUE;
//    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
//    depthStencil.depthBoundsTestEnable = VK_FALSE;
//    depthStencil.stencilTestEnable = VK_FALSE;
//
//    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//    colorBlendAttachment.colorWriteMask =
//        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//    colorBlendAttachment.blendEnable = VK_FALSE;
//
//    VkPipelineColorBlendStateCreateInfo colorBlending{};
//    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//    colorBlending.logicOpEnable = VK_FALSE;
//    colorBlending.logicOp = VK_LOGIC_OP_COPY;
//    colorBlending.attachmentCount = 1;
//    colorBlending.pAttachments = &colorBlendAttachment;
//    colorBlending.blendConstants[0] = 0.0f;
//    colorBlending.blendConstants[1] = 0.0f;
//    colorBlending.blendConstants[2] = 0.0f;
//    colorBlending.blendConstants[3] = 0.0f;
//
//    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
//    VkPipelineDynamicStateCreateInfo dynamicState{};
//    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//    dynamicState.pDynamicStates = dynamicStates.data();
//
//    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//    pipelineLayoutInfo.setLayoutCount = 1;
//    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
//
//    VkResult pipelineResult = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
//    BEET_ASSERT_MESSAGE(pipelineResult == VK_SUCCESS, "failed to create pipeline layout!");
//
//    VkGraphicsPipelineCreateInfo pipelineInfo{};
//    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//    pipelineInfo.stageCount = 2;
//    pipelineInfo.pStages = shaderStages;
//    pipelineInfo.pVertexInputState = &vertexInputInfo;
//    pipelineInfo.pInputAssemblyState = &inputAssembly;
//    pipelineInfo.pViewportState = &viewportState;
//    pipelineInfo.pRasterizationState = &rasterizer;
//    pipelineInfo.pMultisampleState = &multisampling;
//    pipelineInfo.pDepthStencilState = &depthStencil;
//    pipelineInfo.pColorBlendState = &colorBlending;
//    pipelineInfo.pDynamicState = &dynamicState;
//    pipelineInfo.layout = m_pipelineLayout;
//    pipelineInfo.renderPass = m_renderPass;
//    pipelineInfo.subpass = 0;
//    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//
//    VkResult graphicsPipelineResult =
//        vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
//    BEET_ASSERT_MESSAGE(graphicsPipelineResult == VK_SUCCESS, "failed to create graphics pipeline!")
//
//    vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
//    vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
//}
//
// VkShaderModule Device::create_shader_module(const std::vector<char>& code) {
//    VkShaderModuleCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//    createInfo.codeSize = code.size();
//    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
//
//    VkShaderModule shaderModule;
//    VkResult shaderResult = vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);
//    BEET_ASSERT_MESSAGE(shaderResult == VK_SUCCESS, "failed to create shader module!")
//
//    return shaderModule;
//}
//
// void Device::create_instance() {
//    bool validationResult = (ENABLED_VALIDATION_LAYERS && check_validation_layer_support());
//    BEET_ASSERT_MESSAGE(validationResult, "validation layers requested, but not available!");
//
//    VkApplicationInfo appInformation{};
//    appInformation.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//    appInformation.pApplicationName = "vk_beetroot";
//    appInformation.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
//    appInformation.pEngineName = "beetroot";
//    appInformation.engineVersion = VK_MAKE_VERSION(0, 1, 0);
//    appInformation.apiVersion = VK_API_VERSION_1_0;
//
//    VkInstanceCreateInfo createInformation{};
//    createInformation.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//    createInformation.pApplicationInfo = &appInformation;
//
//    auto extensions = get_required_extensions();
//    createInformation.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
//    createInformation.ppEnabledExtensionNames = extensions.data();
//
//    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
//
//    if (ENABLED_VALIDATION_LAYERS) {
//        createInformation.enabledLayerCount = static_cast<uint32_t>(BEET_VK_VALIDATION_LAYERS.size());
//        createInformation.ppEnabledLayerNames = BEET_VK_VALIDATION_LAYERS.data();
//
//        populate_debug_messenger_create_info(debugCreateInfo);
//        createInformation.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
//    } else {
//        createInformation.enabledLayerCount = 0;
//        createInformation.pNext = nullptr;
//    }
//
//    VkResult instanceResult = vkCreateInstance(&createInformation, nullptr, &m_instance);
//    BEET_ASSERT_MESSAGE(instanceResult == VK_SUCCESS, "failed to create instance!")
//}
//
// QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice device) {
//    QueueFamilyIndices indices;
//
//    uint32_t queueFamilyCount = 0;
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//    int i = 0;
//    for (const auto& queueFamily : queueFamilies) {
//        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//            indices.graphicsFamily = i;
//        }
//
//        VkBool32 presentSupport = false;
//        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
//
//        if (presentSupport) {
//            indices.presentFamily = i;
//        }
//
//        if (indices.is_complete()) {
//            break;
//        }
//
//        i++;
//    }
//
//    return indices;
//}
//
// bool Device::is_device_suitable(VkPhysicalDevice device) {
//    QueueFamilyIndices indices = find_queue_families(device);
//
//    bool extensionsSupported = check_device_extension_support(device);
//
//    bool swapChainAdequate = false;
//    if (extensionsSupported) {
//        SwapChainSupportDetails swapChainSupport = query_swap_chain_support(device);
//        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//    }
//
//    VkPhysicalDeviceFeatures supportedFeatures;
//    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
//
//    return indices.is_complete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
//}
//
// bool Device::check_device_extension_support(VkPhysicalDevice device) {
//    uint32_t extensionCount;
//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
//
//    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
//
//    std::set<std::string> requiredExtensions(BEET_VK_DEVICE_EXTENSIONS.begin(), BEET_VK_DEVICE_EXTENSIONS.end());
//
//    for (const auto& extension : availableExtensions) {
//        requiredExtensions.erase(extension.extensionName);
//    }
//
//    return requiredExtensions.empty();
//}
//
// void Device::pick_physical_device() {
//    // TODO select device from score + manual select
//    uint32_t deviceCount = 0;
//    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
//    BEET_ASSERT_MESSAGE(deviceCount, "failed to find GPUs with Vulkan support!");
//
//    std::vector<VkPhysicalDevice> devices(deviceCount);
//    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
//
//    for (const auto& device : devices) {
//        if (is_device_suitable(device)) {
//            VkPhysicalDeviceProperties properties;
//            vkGetPhysicalDeviceProperties(device, &properties);
//            log::debug("Physical device: {}", properties.deviceName);
//            m_physicalDevice = device;
//            break;
//        }
//    }
//
//    BEET_ASSERT_MESSAGE(m_physicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU!");
//}
//
// void Device::create_logical_device() {
//    QueueFamilyIndices indices = find_queue_families(m_physicalDevice);
//
//    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
//
//    float queuePriority = 1.0f;
//    for (uint32_t queueFamily : uniqueQueueFamilies) {
//        VkDeviceQueueCreateInfo queueCreateInfo{};
//        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//        queueCreateInfo.queueFamilyIndex = queueFamily;
//        queueCreateInfo.queueCount = 1;
//        queueCreateInfo.pQueuePriorities = &queuePriority;
//        queueCreateInfos.push_back(queueCreateInfo);
//    }
//
//    VkPhysicalDeviceFeatures deviceFeatures{};
//    deviceFeatures.samplerAnisotropy = VK_TRUE;
//
//    VkDeviceCreateInfo createInformation{};
//    createInformation.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//
//    createInformation.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//    createInformation.pQueueCreateInfos = queueCreateInfos.data();
//
//    createInformation.pEnabledFeatures = &deviceFeatures;
//
//    createInformation.enabledExtensionCount = static_cast<uint32_t>(BEET_VK_DEVICE_EXTENSIONS.size());
//    createInformation.ppEnabledExtensionNames = BEET_VK_DEVICE_EXTENSIONS.data();
//
//    if (ENABLED_VALIDATION_LAYERS) {
//        createInformation.enabledLayerCount = static_cast<uint32_t>(BEET_VK_VALIDATION_LAYERS.size());
//        createInformation.ppEnabledLayerNames = BEET_VK_VALIDATION_LAYERS.data();
//    } else {
//        createInformation.enabledLayerCount = 0;
//    }
//
//    VkResult deviceResult = vkCreateDevice(m_physicalDevice, &createInformation, nullptr, &m_device);
//    BEET_ASSERT_MESSAGE(deviceResult == VK_SUCCESS, "failed to create logical device!")
//
//    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
//    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
//}
//
// std::vector<const char*> Device::get_required_extensions() {
//    uint32_t glfwExtensionCount = 0;
//    const char** glfwExtensions;
//    glfwExtensions = m_engine.get_window_module().lock()->get_extensions(glfwExtensionCount);
//
//    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
//
//    if (ENABLED_VALIDATION_LAYERS) {
//        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//    }
//
//    return extensions;
//}
//
// void Device::setup_debug_messenger() {
//    if (!ENABLED_VALIDATION_LAYERS) {
//        return;
//    }
//
//    VkDebugUtilsMessengerCreateInfoEXT createInformation;
//    populate_debug_messenger_create_info(createInformation);
//
//    VkResult debuggerResult =
//        create_debug_utils_messenger_EXT(m_instance, &createInformation, nullptr, &m_debugMessenger);
//    BEET_ASSERT_MESSAGE(debuggerResult == VK_SUCCESS, "failed to set up debug messenger!")
//}
//
// void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInformation) {
//    createInformation = {};
//    createInformation.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//    createInformation.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
//                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//    createInformation.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
//                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
//                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//    createInformation.pfnUserCallback = debug_callback;
//}
//
// bool Device::check_validation_layer_support() {
//    uint32_t layerCount;
//    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
//
//    std::vector<VkLayerProperties> availableLayers(layerCount);
//    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
//
//    for (const char* layerName : BEET_VK_VALIDATION_LAYERS) {
//        bool layerFound = false;
//
//        for (const auto& layerProperties : availableLayers) {
//            if (strcmp(layerName, layerProperties.layerName) == 0) {
//                layerFound = true;
//                break;
//            }
//        }
//
//        if (!layerFound) {
//            return false;
//        }
//    }
//
//    return true;
//}
//
// void Device::create_surface() {
//    m_engine.get_window_module().lock().get()->create_surface(m_instance, m_surface);
//}
//
// VkSurfaceFormatKHR Device::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
//    for (const auto& availableFormat : availableFormats) {
//        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
//            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//            return availableFormat;
//        }
//    }
//
//    return availableFormats[0];
//}
//
// VkPresentModeKHR Device::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
//    for (const auto& availablePresentMode : availablePresentModes) {
//        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
//            return availablePresentMode;
//        }
//    }
//
//    return VK_PRESENT_MODE_FIFO_KHR;
//}
//
// VkExtent2D Device::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
//    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//        return capabilities.currentExtent;
//    } else {
//        vec2i size = m_engine.get_window_module().lock()->get_window_size();
//
//        VkExtent2D actualExtent = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
//
//        actualExtent.width =
//            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//        actualExtent.height =
//            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
//
//        return actualExtent;
//    }
//}
//
// SwapChainSupportDetails Device::query_swap_chain_support(VkPhysicalDevice device) {
//    SwapChainSupportDetails details;
//
//    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
//
//    uint32_t formatCount;
//    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
//
//    if (formatCount != 0) {
//        details.formats.resize(formatCount);
//        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
//    }
//
//    uint32_t presentModeCount;
//    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
//
//    if (presentModeCount != 0) {
//        details.presentModes.resize(presentModeCount);
//        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
//    }
//    return details;
//}
// void Device::create_swap_chain() {
//    SwapChainSupportDetails swapChainSupport = query_swap_chain_support(m_physicalDevice);
//
//    VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats);
//    VkPresentModeKHR presentMode = choose_swap_present_mode(swapChainSupport.presentModes);
//    VkExtent2D extent = choose_swap_extent(swapChainSupport.capabilities);
//
//    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
//
//    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
//        imageCount = swapChainSupport.capabilities.maxImageCount;
//    }
//
//    VkSwapchainCreateInfoKHR createInformation{};
//    createInformation.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//    createInformation.surface = m_surface;
//
//    createInformation.minImageCount = imageCount;
//    createInformation.imageFormat = surfaceFormat.format;
//    createInformation.imageColorSpace = surfaceFormat.colorSpace;
//    createInformation.imageExtent = extent;
//    createInformation.imageArrayLayers = 1;
//    createInformation.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//
//    QueueFamilyIndices indices = find_queue_families(m_physicalDevice);
//    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
//
//    if (indices.graphicsFamily != indices.presentFamily) {
//        createInformation.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//        createInformation.queueFamilyIndexCount = 2;
//        createInformation.pQueueFamilyIndices = queueFamilyIndices;
//    } else {
//        createInformation.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//        createInformation.queueFamilyIndexCount = 0;      // Optional
//        createInformation.pQueueFamilyIndices = nullptr;  // Optional
//    }
//
//    createInformation.preTransform = swapChainSupport.capabilities.currentTransform;
//    createInformation.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//    createInformation.presentMode = presentMode;
//    createInformation.clipped = VK_TRUE;
//
//    createInformation.oldSwapchain = VK_NULL_HANDLE;
//
//    VkResult swapchainResult = vkCreateSwapchainKHR(m_device, &createInformation, nullptr, &m_swapChain);
//    BEET_ASSERT_MESSAGE(swapchainResult == VK_SUCCESS, "failed to create swap chain!");
//
//    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
//    m_swapChainImages.resize(imageCount);
//    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());
//
//    m_swapChainImageFormat = surfaceFormat.format;
//    m_swapChainExtent = extent;
//}
// void Device::create_image_views() {
//    m_swapChainImageViews.resize(m_swapChainImages.size());
//
//    for (size_t i = 0; i < m_swapChainImages.size(); i++) {
//        m_swapChainImageViews[i] =
//            create_image_view(m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
//    }
//}
//
// VkResult create_debug_utils_messenger_EXT(VkInstance instance,
//                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
//                                          const VkAllocationCallbacks* pAllocator,
//                                          VkDebugUtilsMessengerEXT* pDebugMessenger) {
//    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//    if (func != nullptr) {
//        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//    } else {
//        return VK_ERROR_EXTENSION_NOT_PRESENT;
//    }
//}
//
// void destroy_debug_utils_messengerEXT(VkInstance instance,
//                                      VkDebugUtilsMessengerEXT debugMessenger,
//                                      const VkAllocationCallbacks* pAllocator) {
//    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
//    "vkDestroyDebugUtilsMessengerEXT"); if (func != nullptr) {
//        func(instance, debugMessenger, pAllocator);
//    }
//}
//
// static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
//                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//                                                     void* pUserData) {
//    std::string prefix{};
//    switch (messageType) {
//        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
//            prefix = "[GENERAL]: ";
//            break;
//        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
//            prefix = "[VALIDATION]: ";
//            break;
//        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
//            prefix = "[PERFORMANCE]: ";
//            break;
//        default:
//            prefix = "[UNKNOWN]: ";
//            break;
//    }
//    switch (messageSeverity) {
//        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
//#if DEVICE_VERBOSE_LOGGING
//            log::debug("{}{}", prefix, pCallbackData->pMessage);
//#endif
//            break;
//        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
//#if DEVICE_VERBOSE_LOGGING
//            log::warn("{}{}", prefix, pCallbackData->pMessage);
//#endif
//            break;
//        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
//            log::error("{}{}", prefix, pCallbackData->pMessage);
//            break;
//        default:
//            log::info("{}{}", prefix, pCallbackData->pMessage);
//            break;
//    }
//
//    return VK_FALSE;
//}
//}  // namespace beet
//
//// namespace beet
