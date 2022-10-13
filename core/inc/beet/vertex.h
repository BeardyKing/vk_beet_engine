#pragma once

#include <beet/types.h>

#include <vulkan/vulkan.h>

#include <array>
namespace beet {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription get_binding_description();
    static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions();
};
}  // namespace beet
