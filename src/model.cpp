#include "lynx/pch.hpp"
#include "lynx/model.hpp"
#include "lynx/vertex.hpp"

namespace lynx
{
model::model(const ref<const device> &dev, const std::vector<vertex2D> &vertices)
    : m_vertex_buffer(dev, vertices), m_vertex_count(vertices.size())
{
}

model::model(const ref<const device> &dev, const std::vector<vertex3D> &vertices)
    : m_vertex_buffer(dev, vertices), m_vertex_count(vertices.size())
{
}

void model::bind(VkCommandBuffer command_buffer) const
{
    m_vertex_buffer.bind(command_buffer);
}
void model::draw(VkCommandBuffer command_buffer) const
{
    vkCmdDraw(command_buffer, (std::uint32_t)m_vertex_count, 1, 0, 0);
}

model2D::model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices) : model(dev, vertices)
{
}

const std::vector<vertex2D> &model2D::triangle(const glm::vec3 &color)
{
    static std::vector<vertex2D> vertices = {{{0.f, -0.25f}, color}, {{0.25f, 0.25f}, color}, {{-0.25f, 0.25f}, color}};
    return vertices;
}

const std::vector<vertex2D> &model2D::line(const glm::vec3 &color)
{
    static std::vector<vertex2D> vertices = {{{-1.f, 0.f}, color}, {{1.f, 0.f}, color}};
    return vertices;
}

model3D::model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices) : model(dev, vertices)
{
}

const std::vector<vertex3D> &model3D::cube(const glm::vec3 &color)
{
    static std::vector<vertex3D> vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    };
    return vertices;
}
} // namespace lynx