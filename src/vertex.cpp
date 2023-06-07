#include "lynx/pch.hpp"
#include "lynx/vertex.hpp"

namespace lynx
{
std::vector<VkVertexInputBindingDescription> vertex2D::binding_descriptions()
{
    return {{0, sizeof(vertex2D), VK_VERTEX_INPUT_RATE_VERTEX}};
}
std::vector<VkVertexInputAttributeDescription> vertex2D::attribute_descriptions()
{
    return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex2D, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex2D, color)}};
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