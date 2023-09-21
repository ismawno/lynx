#include "lynx/internal/pch.hpp"
#include "lynx/geometry/vertex.hpp"

namespace lynx
{
template <typename Dim>
vertex<Dim>::vertex(const vec_t &position, const lynx::color &pcolor) : position(position), color(pcolor)
{
}
template <typename Dim> std::vector<VkVertexInputBindingDescription> vertex<Dim>::binding_descriptions()
{
    return {{0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}
template <typename Dim> std::vector<VkVertexInputAttributeDescription> vertex<Dim>::attribute_descriptions()
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
        return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, position)},
                {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vertex, color)}};
    else
        return {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex3D, position)},
                {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vertex3D, color)}};
}
} // namespace lynx