#include "lynx/pch.hpp"
#include "lynx/model.hpp"
#include "lynx/vertex.hpp"

namespace lynx
{
template <typename T>
model::model(const ref<const device> &dev, const std::vector<T> &vertices)
    : m_vertex_buffer(dev, vertices), m_vertex_count(vertices.size())
{
}

template <typename T>
model::model(const ref<const device> &dev, const std::vector<T> &vertices, const std::vector<std::uint32_t> &indices)
    : m_vertex_buffer(dev, vertices, indices), m_vertex_count(vertices.size()), m_index_count(indices.size())
{
}

template <typename T>
model::model(const ref<const device> &dev, const vertex_index_pair<T> &build)
    : model(dev, build.vertices, build.indices)
{
}

void model::bind(VkCommandBuffer command_buffer) const
{
    m_vertex_buffer.bind(command_buffer);
}
void model::draw(VkCommandBuffer command_buffer) const
{
    if (m_vertex_buffer.has_index_buffer())
        vkCmdDrawIndexed(command_buffer, (std::uint32_t)m_index_count, 1, 0, 0, 0);
    else
        vkCmdDraw(command_buffer, (std::uint32_t)m_vertex_count, 1, 0, 0);
}

model2D::model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices) : model(dev, vertices)
{
}

model2D::model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices,
                 const std::vector<std::uint32_t> &indices)
    : model(dev, vertices, indices)
{
}

model2D::model2D(const ref<const device> &dev, const vertex_index_pair &build) : model(dev, build)
{
}

const model2D::vertex_index_pair &model2D::rect(const glm::vec3 &color)
{
    static const std::vector<vertex2D> vertices = {
        {{-.5f, -.5f}, color}, {{.5f, .5f}, color}, {{-.5f, .5f}, color}, {{.5f, -.5f}, color}};

    static const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 3, 1};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const std::vector<vertex2D> &model2D::line(const glm::vec3 &color)
{
    static std::vector<vertex2D> vertices = {{{-1.f, 0.f}, color}, {{1.f, 0.f}, color}};
    return vertices;
}

model3D::model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices) : model(dev, vertices)
{
}

model3D::model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices,
                 const std::vector<std::uint32_t> &indices)
    : model(dev, vertices, indices)
{
}

model3D::model3D(const ref<const device> &dev, const vertex_index_pair &build) : model(dev, build)
{
}

const model3D::vertex_index_pair &model3D::rect(const glm::vec3 &color)
{
    static std::vector<vertex3D> vertices = {
        {{-.5f, -.5f, .5f}, color}, {{.5f, .5f, .5f}, color}, {{-.5f, .5f, .5f}, color}, {{.5f, -.5f, .5f}, color}};

    static const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 3, 1};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const model3D::vertex_index_pair &model3D::cube(const glm::vec3 &color)
{
    static const std::vector<vertex3D> vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    static const std::vector<std::uint32_t> indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,
                                                       8,  9,  10, 8,  11, 9,  12, 13, 14, 12, 15, 13,
                                                       16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const std::vector<vertex3D> &model3D::line(const glm::vec3 &color)
{
    static std::vector<vertex3D> vertices = {{{-1.f, 0.f, 0.f}, color}, {{1.f, 0.f, 0.f}, color}};
    return vertices;
}

} // namespace lynx