#pragma once

#include <beet/types.h>

#include <vulkan/vulkan.h>

#include <array>
namespace beet {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription get_binding_description();
    static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions();
};
}  // namespace beet
