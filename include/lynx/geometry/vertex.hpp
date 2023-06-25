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
    vertex2D() = default;
    vertex2D(const glm::vec2 &position, const glm::vec4 &color);
    glm::vec2 position;
    glm::vec4 color;

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};

struct vertex3D
{
    vertex3D() = default;
    vertex3D(const glm::vec3 &position, const glm::vec4 &color);
    glm::vec3 position;
    glm::vec4 color;

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};
} // namespace lynx

#endif