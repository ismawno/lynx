#ifndef LYNX_VERTEX_HPP
#define LYNX_VERTEX_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vulkan/vulkan.hpp>

namespace lynx
{
struct vertex2D
{
    glm::vec2 position;
    glm::vec4 color;

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};

struct vertex3D
{
    glm::vec3 position;
    glm::vec4 color;

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};
} // namespace lynx

#endif