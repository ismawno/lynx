#include "lynx/pch.hpp"
#include "lynx/model.hpp"
#include "lynx/vertex.hpp"
#include "lynx/buffer.hpp"

namespace lynx
{
template <typename T> model::model(const ref<const device> &dev, const std::vector<T> &vertices)
{
    DBG_ASSERT_ERROR(!vertices.empty(), "Cannot create a model with no vertices")
    create_vertex_buffer(dev, vertices);
    m_device_index_buffer = nullptr;
    m_host_index_buffer = nullptr;
}

template <typename T>
model::model(const ref<const device> &dev, const std::vector<T> &vertices, const std::vector<std::uint32_t> &indices)
{
    DBG_ASSERT_ERROR(!vertices.empty(), "Cannot create a model with no vertices")
    DBG_ASSERT_ERROR(!indices.empty(), "If specified, indices must not be empty")
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
    host_buffer = make_scope<buffer>(dev, sizeof(T), data.size(), (VkBufferUsageFlags)VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                     (VkMemoryPropertyFlags)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    host_buffer->map();
    host_buffer->write((void *)data.data());
    host_buffer->flush();

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
    if (has_index_buffers())
        vkCmdDrawIndexed(command_buffer, (std::uint32_t)m_device_index_buffer->instance_count(), 1, 0, 0, 0);
    else
        vkCmdDraw(command_buffer, (std::uint32_t)m_device_vertex_buffer->instance_count(), 1, 0, 0);
}

bool model::has_index_buffers() const
{
    return m_device_index_buffer && m_host_index_buffer;
}

template <typename T>
static void update_buffer(std::function<void(T &)> for_each_fn, buffer &host_buffer, buffer &device_buffer)
{
    if (for_each_fn)
    {
        for (std::size_t i = 0; i < host_buffer.instance_count(); i++)
            for_each_fn(host_buffer.read_at_index<T>(i));
        host_buffer.flush();
    }
    device_buffer.write(host_buffer);
}

template <typename T> void model::update_vertex_buffer(std::function<void(T &)> for_each_fn)
{
    update_buffer(for_each_fn, *m_host_vertex_buffer, *m_device_vertex_buffer);
}
void model::update_index_buffer(std::function<void(std::uint32_t &)> for_each_fn)
{
    DBG_ASSERT_ERROR(has_index_buffers(), "Cannot update index buffer in a model that does not have index buffers")
    update_buffer(for_each_fn, *m_host_index_buffer, *m_device_index_buffer);
}

template <typename T> void model::write_vertex(const std::size_t buffer_index, const T &vertex)
{
    m_host_vertex_buffer->write_at_index(&vertex, buffer_index);
    m_host_vertex_buffer->flush_at_index(buffer_index);
    update_vertex_buffer<T>();
}

void model::write_index(const std::size_t buffer_index, const std::uint32_t index)
{
    DBG_ASSERT_ERROR(has_index_buffers(), "Cannot write to index buffer in a model that does not have index buffers")
    m_host_index_buffer->write_at_index(&index, buffer_index);
    m_host_index_buffer->flush_at_index(buffer_index);
    update_index_buffer();
}

template <typename T> const T &model::read_vertex(const std::size_t buffer_index) const
{
    return m_host_vertex_buffer->read_at_index<T>(buffer_index);
}

std::uint32_t model::read_index(const std::size_t buffer_index) const
{
    DBG_ASSERT_ERROR(has_index_buffers(), "Cannot write to index buffer in a model that does not have index buffers")
    return m_host_index_buffer->read_at_index<std::uint32_t>(buffer_index);
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

void model2D::write_vertex(std::size_t buffer_index, const vertex2D &vertex)
{
    model::write_vertex(buffer_index, vertex);
}

const vertex2D &model2D::read_vertex(std::size_t buffer_index) const
{
    return model::read_vertex<vertex2D>(buffer_index);
}

void model2D::update_vertex_buffer(std::function<void(vertex2D &)> for_each_fn)
{
    model::update_vertex_buffer(for_each_fn);
}

const vertex2D &model2D::operator[](const std::size_t index) const
{
    return read_vertex(index);
}

const model2D::vertex_index_pair &model2D::rect(const glm::vec4 &color)
{
    static const std::vector<vertex2D> vertices = {
        {{-.5f, -.5f}, color}, {{.5f, .5f}, color}, {{-.5f, .5f}, color}, {{.5f, -.5f}, color}};

    static const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 3, 1};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const std::vector<vertex2D> &model2D::line(const glm::vec4 &color1, const glm::vec4 &color2)
{
    static std::vector<vertex2D> vertices = {{{-1.f, 0.f}, color1}, {{1.f, 0.f}, color2}};
    return vertices;
}

model2D::vertex_index_pair model2D::circle(const std::uint32_t partitions, const glm::vec4 &color)
{
    DBG_ASSERT_ERROR(partitions > 2, "Must at least have 3 partitions")

    std::vector<vertex2D> vertices = {{{0.f, 0.f}, color}};
    std::vector<std::uint32_t> indices(partitions * 3);
    vertices.reserve(partitions);

    const float dangle = 2.f * glm::pi<float>() / (partitions - 1);
    for (std::uint32_t i = 0; i < partitions; i++)
    {
        const float angle = i * dangle;
        vertices.emplace_back(glm::vec2(cosf(angle), sinf(angle)), color);
        indices[3 * i] = 0;
        indices[3 * i + 1] = i;
        indices[3 * i + 2] = i + 1;
    }
    return {vertices, indices};
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

void model3D::write_vertex(std::size_t buffer_index, const vertex3D &vertex)
{
    model::write_vertex(buffer_index, vertex);
}
const vertex3D &model3D::read_vertex(std::size_t buffer_index) const
{
    return model::read_vertex<vertex3D>(buffer_index);
}

void model3D::update_vertex_buffer(std::function<void(vertex3D &)> for_each_fn)
{
    model::update_vertex_buffer(for_each_fn);
}

const vertex3D &model3D::operator[](const std::size_t index) const
{
    return read_vertex(index);
}

const model3D::vertex_index_pair &model3D::rect(const glm::vec4 &color)
{
    static std::vector<vertex3D> vertices = {
        {{-.5f, -.5f, .5f}, color}, {{.5f, .5f, .5f}, color}, {{-.5f, .5f, .5f}, color}, {{.5f, -.5f, .5f}, color}};

    static const std::vector<std::uint32_t> indices = {0, 1, 2, 0, 3, 1};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const model3D::vertex_index_pair &model3D::cube(const std::array<glm::vec4, 6> &face_colors)
{
    static const std::vector<vertex3D> vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, face_colors[0]},
        {{-.5f, .5f, .5f}, face_colors[0]},
        {{-.5f, -.5f, .5f}, face_colors[0]},
        {{-.5f, .5f, -.5f}, face_colors[0]},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, face_colors[1]},
        {{.5f, .5f, .5f}, face_colors[1]},
        {{.5f, -.5f, .5f}, face_colors[1]},
        {{.5f, .5f, -.5f}, face_colors[1]},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, face_colors[2]},
        {{.5f, -.5f, .5f}, face_colors[2]},
        {{-.5f, -.5f, .5f}, face_colors[2]},
        {{.5f, -.5f, -.5f}, face_colors[2]},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, face_colors[3]},
        {{.5f, .5f, .5f}, face_colors[3]},
        {{-.5f, .5f, .5f}, face_colors[3]},
        {{.5f, .5f, -.5f}, face_colors[3]},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, face_colors[4]},
        {{.5f, .5f, 0.5f}, face_colors[4]},
        {{-.5f, .5f, 0.5f}, face_colors[4]},
        {{.5f, -.5f, 0.5f}, face_colors[4]},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, face_colors[5]},
        {{.5f, .5f, -0.5f}, face_colors[5]},
        {{-.5f, .5f, -0.5f}, face_colors[5]},
        {{.5f, -.5f, -0.5f}, face_colors[5]},
    };

    static const std::vector<std::uint32_t> indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,
                                                       8,  9,  10, 8,  11, 9,  12, 13, 14, 12, 15, 13,
                                                       16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
    static const vertex_index_pair build = {vertices, indices};
    return build;
}

const std::vector<vertex3D> &model3D::line(const glm::vec4 &color1, const glm::vec4 &color2)
{
    static std::vector<vertex3D> vertices = {{{-1.f, 0.f, 0.f}, color1}, {{1.f, 0.f, 0.f}, color2}};
    return vertices;
}

} // namespace lynx