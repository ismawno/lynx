#include "lynx/pch.hpp"
#include "lynx/vertex.hpp"

namespace lynx
{
vertex2D::vertex2D(const glm::vec2 &pposition, const glm::vec4 &pcolor) : position(pposition), color(pcolor)
{
}
std::vector<VkVertexInputBindingDescription> vertex2D::binding_descriptions()
{
    return {{0, sizeof(vertex2D), VK_VERTEX_INPUT_RATE_VERTEX}};
}
std::vector<VkVertexInputAttributeDescription> vertex2D::attribute_descriptions()
{
    return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex2D, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex2D, color)}};
}

vertex3D::vertex3D(const glm::vec3 &pposition, const glm::vec4 &pcolor) : position(pposition), color(pcolor)
{
}

std::vector<VkVertexInputBindingDescription> vertex3D::binding_descriptions()
{
    return {{0, sizeof(vertex3D), VK_VERTEX_INPUT_RATE_VERTEX}};
}
std::vector<VkVertexInputAttributeDescription> vertex3D::attribute_descriptions()
{
    return {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex3D, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex3D, color)}};
}
} // namespace lynx