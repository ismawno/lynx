#include "lynx/pch.hpp"
#include "lynx/model.hpp"

namespace lynx
{
template <typename T>
model::model(const device &dev, const std::vector<T> &vertices)
    : m_vertex_buffer(dev, vertices), m_vertex_count(vertices.size())
{
}
template model::model(const device &dev, const std::vector<model2D::vertex> &vertices);
template model::model(const device &dev, const std::vector<model3D::vertex> &vertices);

void model::bind(VkCommandBuffer command_buffer) const
{
    m_vertex_buffer.bind(command_buffer);
}
void model::draw(VkCommandBuffer command_buffer) const
{
    vkCmdDraw(command_buffer, (std::uint32_t)m_vertex_count, 1, 0, 0);
}

model2D::model2D(const device &dev, const std::vector<vertex> &vertices) : model(dev, vertices)
{
}

std::vector<VkVertexInputBindingDescription> model2D::vertex::binding_descriptions()
{
    return {{0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}
std::vector<VkVertexInputAttributeDescription> model2D::vertex::attribute_descriptions()
{
    return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, color)}};
}

model3D::model3D(const device &dev, const std::vector<vertex> &vertices) : model(dev, vertices)
{
}

std::vector<VkVertexInputBindingDescription> model3D::vertex::binding_descriptions()
{
    return {{0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}
std::vector<VkVertexInputAttributeDescription> model3D::vertex::attribute_descriptions()
{
    return {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, color)}};
}

} // namespace lynx