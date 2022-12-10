#pragma once

#include <beet/gfx/types.h>
#include <beet/gfx/vulkan_buffer.h>
#include <beet/types.h>

#include <vulkan/vulkan.h>

namespace beet::gfx {
struct VertexInputDescription {
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VertexInputDescription get_vertex_description();
};

struct Mesh {
    std::vector<Vertex> vertices;
    AllocatedBuffer vertexBuffer;
};

}  // namespace beet::gfx
