#include "lynx/pch.hpp"
#include "lynx/model.hpp"
#include "lynx/vertex.hpp"
#include "lynx/buffer.hpp"

namespace lynx
{
template <typename T> model::model(const ref<const device> &dev, const std::vector<T> &vertices)
{
    create_vertex_buffer(dev, vertices);
}

template <typename T>
model::model(const ref<const device> &dev, const std::vector<T> &vertices, const std::vector<std::uint32_t> &indices)
{
    create_vertex_buffer(dev, vertices);
    create_index_buffer(dev, indices);
}

template <typename T>
model::model(const ref<const device> &dev, const vertex_index_pair<T> &build)
    : model(dev, build.vertices, build.indices)
{
}

template <typename T>
static void create_buffer(const ref<const device> &dev, const std::vector<T> &data, VkBufferUsageFlags usage,
                          scope<buffer> &device_buffer, scope<buffer> &host_buffer)
{
    host_buffer = make_scope<buffer>(
        dev, sizeof(T), data.size(), (VkBufferUsageFlags)VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        (VkMemoryPropertyFlags)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    host_buffer->map();
    host_buffer->write((void *)data.data());

    device_buffer = make_scope<buffer>(dev, sizeof(T), data.size(), usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    device_buffer->write(*host_buffer);
}

template <typename T> void model::create_vertex_buffer(const ref<const device> &dev, const std::vector<T> &vertices)
{
    create_buffer(dev, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_device_vertex_buffer, m_host_vertex_buffer);
}

void model::create_index_buffer(const ref<const device> &dev, const std::vector<std::uint32_t> &indices)
{
    create_buffer(dev, indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_device_index_buffer, m_host_index_buffer);
}

void model::bind(VkCommandBuffer command_buffer) const
{
    const std::array<VkBuffer, 1> buffers = {m_device_vertex_buffer->vulkan_buffer()};
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());

    if (m_device_index_buffer)
        vkCmdBindIndexBuffer(command_buffer, m_device_index_buffer->vulkan_buffer(), 0, VK_INDEX_TYPE_UINT32);
}
void model::draw(VkCommandBuffer command_buffer) const
{
    if (m_device_index_buffer && m_host_index_buffer)
        vkCmdDrawIndexed(command_buffer, (std::uint32_t)m_device_index_buffer->instance_count(), 1, 0, 0, 0);
    else
        vkCmdDraw(command_buffer, (std::uint32_t)m_device_vertex_buffer->instance_count(), 1, 0, 0);
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

const model2D::vertex_index_pair &model2D::rect(const glm::vec4 &color)
{
    static const std::vector<vertex2D> vertices = {
        {{-.5f, -.5f}, color}, {{.5f, .5f}, color}, {{-.5f, .5f}, color}, {{.5f, -.5f}, color}};

    static const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 3, 1};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const std::vector<vertex2D> &model2D::line(const glm::vec4 &color)
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

const model3D::vertex_index_pair &model3D::rect(const glm::vec4 &color)
{
    static std::vector<vertex3D> vertices = {
        {{-.5f, -.5f, .5f}, color}, {{.5f, .5f, .5f}, color}, {{-.5f, .5f, .5f}, color}, {{.5f, -.5f, .5f}, color}};

    static const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 3, 1};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const model3D::vertex_index_pair &model3D::cube(const glm::vec4 &color)
{
    static const std::vector<vertex3D> vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f, 1.f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f, 1.f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f, 1.f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f, 1.f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f, 1.f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f, 1.f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f, 1.f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f, 1.f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f, 1.f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f, 1.f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f, 1.f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f, 1.f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f, 1.f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f, 1.f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f, 1.f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f, 1.f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f, 1.f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f, 1.f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f, 1.f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f, 1.f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f, 1.f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f, 1.f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f, 1.f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f, 1.f}},
    };

    static const std::vector<std::uint32_t> indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,
                                                       8,  9,  10, 8,  11, 9,  12, 13, 14, 12, 15, 13,
                                                       16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const std::vector<vertex3D> &model3D::line(const glm::vec4 &color)
{
    static std::vector<vertex3D> vertices = {{{-1.f, 0.f, 0.f}, color}, {{1.f, 0.f, 0.f}, color}};
    return vertices;
}

} // namespace lynx