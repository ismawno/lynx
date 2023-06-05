#include "lynx/pch.hpp"
#include "lynx/model.hpp"

namespace lynx
{
model::model(const device &dev, const std::vector<vertex2D> &vertices)
    : m_vertex_buffer(dev, vertices), m_vertex_count(vertices.size())
{
}

model::model(const device &dev, const std::vector<vertex3D> &vertices)
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

model2D::model2D(const device &dev, const std::vector<vertex2D> &vertices) : model(dev, vertices)
{
}

std::vector<vertex2D> model2D::triangle(const glm::vec3 &color)
{
    return {{{0.f, -0.25f}, color}, {{0.25f, 0.25f}, color}, {{-0.25f, 0.25f}, color}};
}

transform2D::operator glm::mat4() const
{
    const float c = cosf(rotation);
    const float s = sinf(rotation);
    return glm::mat4{{
                         scale.x * c,
                         scale.x * s,
                         0.f,
                         0.f,
                     },
                     {
                         -scale.y * s,
                         scale.y * c,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                     },
                     {translation.x, translation.y, 0.f, 1.0f}};
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

model3D::model3D(const device &dev, const std::vector<vertex3D> &vertices) : model(dev, vertices)
{
}

std::vector<vertex3D> model3D::cube(const glm::vec3 &color)
{
    return {
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

transform3D::operator glm::mat4() const
{
    const float c3 = cosf(rotation.z);
    const float s3 = sinf(rotation.z);
    const float c2 = cosf(rotation.x);
    const float s2 = sinf(rotation.x);
    const float c1 = cosf(rotation.y);
    const float s1 = sinf(rotation.y);
    return glm::mat4{{
                         scale.x * (c1 * c3 + s1 * s2 * s3),
                         scale.x * (c2 * s3),
                         scale.x * (c1 * s2 * s3 - c3 * s1),
                         0.f,
                     },
                     {
                         scale.y * (c3 * s1 * s2 - c1 * s3),
                         scale.y * (c2 * c3),
                         scale.y * (c1 * c3 * s2 + s1 * s3),
                         0.f,
                     },
                     {
                         scale.z * (c2 * s1),
                         scale.z * (-s2),
                         scale.z * (c1 * c2),
                         0.f,
                     },
                     {translation.x, translation.y, translation.z, 1.0f}};
}

} // namespace lynx