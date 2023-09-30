#include "lynx/internal/pch.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/geometry/camera.hpp"

namespace lynx
{
template <typename Dim>
model<Dim>::model(const kit::ref<const device> &dev, const std::vector<vertex_t> &vertices) : m_device(dev)
{
    KIT_ASSERT_ERROR(!vertices.empty(), "Cannot create a model with no vertices")
    create_vertex_buffer(vertices);
    m_device_index_buffer = nullptr;
    m_host_index_buffer = nullptr;
}

template <typename Dim>
model<Dim>::model(const kit::ref<const device> &dev, const std::vector<vertex_t> &vertices,
                  const std::vector<std::uint32_t> &indices)
    : m_device(dev)
{
    KIT_ASSERT_ERROR(!vertices.empty(), "Cannot create a model with no vertices")
    KIT_ASSERT_ERROR(!indices.empty(), "If specified, indices must not be empty")
    create_vertex_buffer(vertices);
    create_index_buffer(indices);
}

template <typename Dim>
model<Dim>::model(const kit::ref<const device> &dev, const vertex_index_pair &build)
    : model(dev, build.vertices, build.indices)
{
}

template <typename Dim> model<Dim>::model(const model &other)
{
    copy(other);
}
template <typename Dim> model<Dim> &model<Dim>::operator=(const model &other)
{
    copy(other);
    return *this;
}

template <typename Dim> void model<Dim>::copy(const model &other)
{
    m_device = other.m_device;
    std::vector<vertex_t> vertices;
    vertices.reserve(other.m_host_vertex_buffer->instance_count());

    for (std::size_t i = 0; i < other.m_host_vertex_buffer->instance_count(); i++)
        vertices.push_back(other.read_vertex(i));
    create_vertex_buffer(vertices);

    if (!other.has_index_buffers())
    {
        m_device_index_buffer = nullptr;
        m_host_index_buffer = nullptr;
        return;
    }

    std::vector<std::uint32_t> indices;
    indices.reserve(other.m_host_index_buffer->instance_count());

    for (std::size_t i = 0; i < other.m_host_index_buffer->instance_count(); i++)
        indices.push_back(other.read_index(i));
    create_index_buffer(indices);
}

#ifdef DEBUG
template <typename Dim> model<Dim>::~model()
{
    KIT_ASSERT_CRITICAL(!to_be_rendered,
                        "Model has been destroyed before being rendered! Any drawable entity must remain alive until "
                        "the end of the frame once it has been submitted for drawing")
}
#endif

template <typename T>
static void create_buffer(const kit::ref<const device> &dev, const std::vector<T> &data, VkBufferUsageFlags usage,
                          kit::scope<buffer> &device_buffer, kit::scope<buffer> &host_buffer)
{
    host_buffer =
        kit::make_scope<buffer>(dev, sizeof(T), data.size(), (VkBufferUsageFlags)VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                (VkMemoryPropertyFlags)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
    host_buffer->map();
    host_buffer->write((void *)data.data());
    host_buffer->flush();

    device_buffer = kit::make_scope<buffer>(dev, sizeof(T), data.size(), usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    device_buffer->write(*host_buffer);
}

template <typename Dim> void model<Dim>::create_vertex_buffer(const std::vector<vertex_t> &vertices)
{
    create_buffer(m_device, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_device_vertex_buffer, m_host_vertex_buffer);
}

template <typename Dim> void model<Dim>::create_index_buffer(const std::vector<std::uint32_t> &indices)
{
    create_buffer(m_device, indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_device_index_buffer, m_host_index_buffer);
}

template <typename Dim> void model<Dim>::bind(VkCommandBuffer command_buffer) const
{
    const std::array<VkBuffer, 1> buffers = {m_device_vertex_buffer->vulkan_buffer()};
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());

    if (m_device_index_buffer)
        vkCmdBindIndexBuffer(command_buffer, m_device_index_buffer->vulkan_buffer(), 0, VK_INDEX_TYPE_UINT32);
}
template <typename Dim> void model<Dim>::draw(VkCommandBuffer command_buffer) const
{
    if (has_index_buffers())
        vkCmdDrawIndexed(command_buffer, (std::uint32_t)m_device_index_buffer->instance_count(), 1, 0, 0, 0);
    else
        vkCmdDraw(command_buffer, (std::uint32_t)m_device_vertex_buffer->instance_count(), 1, 0, 0);
}

template <typename Dim> bool model<Dim>::has_index_buffers() const
{
    return m_device_index_buffer && m_host_index_buffer;
}

template <typename T>
static void update_buffer(const std::function<void(std::size_t, T &)> &for_each_fn, buffer &host_buffer,
                          buffer &device_buffer)
{
    if (for_each_fn)
    {
        for (std::size_t i = 0; i < host_buffer.instance_count(); i++)
            for_each_fn(i, host_buffer.read_at_index<T>(i));
        host_buffer.flush();
    }
    device_buffer.write(host_buffer);
}

template <typename Dim>
void model<Dim>::update_vertex_buffer(const std::function<void(std::size_t, vertex_t &)> &for_each_fn)
{
    update_buffer(for_each_fn, *m_host_vertex_buffer, *m_device_vertex_buffer);
}
template <typename Dim>
void model<Dim>::update_index_buffer(const std::function<void(std::size_t, std::uint32_t &)> &for_each_fn)
{
    KIT_ASSERT_ERROR(has_index_buffers(), "Cannot update index buffer in a model that does not have index buffers")
    update_buffer(for_each_fn, *m_host_index_buffer, *m_device_index_buffer);
}

template <typename Dim> std::size_t model<Dim>::vertices_count() const
{
    return m_host_vertex_buffer->instance_count();
}

template <typename Dim> std::size_t model<Dim>::indices_count() const
{
    return m_host_index_buffer ? m_host_index_buffer->instance_count() : 0;
}

template <typename Dim> void model<Dim>::write_vertex(const std::size_t buffer_index, const vertex_t &vertex)
{
    m_host_vertex_buffer->write_at_index(&vertex, buffer_index);
    m_host_vertex_buffer->flush_at_index(buffer_index);
    update_vertex_buffer();
}

template <typename Dim> void model<Dim>::write_index(const std::size_t buffer_index, const std::uint32_t index)
{
    KIT_ASSERT_ERROR(has_index_buffers(), "Cannot write to index buffer in a model that does not have index buffers")
    m_host_index_buffer->write_at_index(&index, buffer_index);
    m_host_index_buffer->flush_at_index(buffer_index);
    update_index_buffer();
}

template <typename Dim> const vertex<Dim> &model<Dim>::read_vertex(const std::size_t buffer_index) const
{
    return m_host_vertex_buffer->read_at_index<vertex_t>(buffer_index);
}

template <typename Dim> const vertex<Dim> &model<Dim>::operator[](const std::size_t index) const
{
    return read_vertex(index);
}

template <typename Dim> std::uint32_t model<Dim>::read_index(const std::size_t buffer_index) const
{
    KIT_ASSERT_ERROR(has_index_buffers(), "Cannot write to index buffer in a model that does not have index buffers")
    return m_host_index_buffer->read_at_index<std::uint32_t>(buffer_index);
}

template <typename Dim> typename model<Dim>::vertex_index_pair model<Dim>::rect(const color &color)
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
    {
        const vertex_index_pair build = {
            {{{-.5f, -.5f}, color}, {{.5f, .5f}, color}, {{-.5f, .5f}, color}, {{.5f, -.5f}, color}},
            {0, 1, 2, 0, 3, 1}};
        return build;
    }
    else
    {
        const vertex_index_pair build = {{{{-.5f, -.5f, .5f}, color},
                                          {{.5f, .5f, .5f}, color},
                                          {{-.5f, .5f, .5f}, color},
                                          {{.5f, -.5f, .5f}, color}},
                                         {0, 1, 2, 0, 3, 1}};
        return build;
    }
}

template <typename Dim> std::vector<vertex<Dim>> model<Dim>::line(const color &color1, const color &color2)
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
        return {{{-1.f, 0.f}, color1}, {{1.f, 0.f}, color2}};
    else
        return {{{-1.f, 0.f, 0.f}, color1}, {{1.f, 0.f, 0.f}, color2}};
}
template <typename Dim>
typename model<Dim>::vertex_index_pair model<Dim>::circle(const std::uint32_t partitions, const color &color)
{
    KIT_ASSERT_ERROR(partitions > 2, "Must at least have 3 partitions. Current: {0}", partitions)

    vertex_index_pair build;
    build.vertices.emplace_back(vertex_t(0.f), color);
    build.indices.resize(3 * partitions);
    build.vertices.reserve(partitions);

    const float dangle = 2.f * glm::pi<float>() / (partitions - 1);
    for (std::uint32_t i = 0; i < partitions; i++)
    {
        const float angle = i * dangle;
        if constexpr (std::is_same_v<vertex_t, glm::vec2>)
            build.vertices.emplace_back(vertex_t(cosf(angle), sinf(angle)), color);
        else
            build.vertices.emplace_back(vertex_t(cosf(angle), sinf(angle), 0.f), color);
        build.indices[3 * i] = 0;
        build.indices[3 * i + 1] = i + 1;
        build.indices[3 * i + 2] = i + 2;
    }
    return build;
}
template <typename Dim>
typename model<Dim>::vertex_index_pair model<Dim>::polygon(const std::vector<vertex_t> &local_vertices,
                                                           const color &center_color)
{
    KIT_ASSERT_ERROR(local_vertices.size() > 2, "Must at least have 3 vertices. Current: {0}", local_vertices.size())
    vertex_index_pair build;
    build.vertices.reserve(local_vertices.size() + 1);

    vertex_t &center_vertex = build.vertices.emplace_back();
    center_vertex.color = center_color;

    build.indices.resize(3 * local_vertices.size());

    for (std::uint32_t i = 0; i < local_vertices.size(); i++)
    {
        build.vertices.emplace_back(local_vertices[i]);
        build.indices[3 * i] = 0;
        build.indices[3 * i + 1] = i + 1;
        build.indices[3 * i + 2] = i + 2;
    }
    build.indices.back() = 1;
    return build;
}
template <typename Dim>
typename model<Dim>::vertex_index_pair model<Dim>::polygon(const std::vector<vec_t> &local_vertices, const color &color)
{
    std::vector<vertex_t> colored_vertices;
    colored_vertices.reserve(local_vertices.size());

    for (const vertex_t &v : local_vertices)
        colored_vertices.emplace_back(v, color);
    return model::polygon(colored_vertices, color);
}

model3D::vertex_index_pair model3D::sphere(const std::uint32_t lat_partitions, const std::uint32_t lon_partitions,
                                           const color &color)
{
    KIT_ASSERT_ERROR(lat_partitions > 2 && lon_partitions > 2,
                     "Must at least have 3 partitions for each angle. lat partitions: {0}, lon_partitions",
                     lat_partitions, lon_partitions)

    vertex_index_pair build;
    build.vertices.reserve(lon_partitions * lat_partitions);
    build.indices.reserve(3 * lon_partitions * lat_partitions + 2);

    const float dlon = glm::pi<float>() / (lon_partitions - 1);
    const float dlat = 2.f * glm::pi<float>() / (lat_partitions - 1);

    build.vertices.emplace_back(glm::vec3(0.f, 0.f, 1.f), color);
    for (std::uint32_t i = 1; i < lon_partitions - 1; i++)
    {
        const float lon = i * dlon;
        const float lon_xy = sinf(lon);
        const float z = cosf(lon);
        for (std::uint32_t j = 0; j < lat_partitions; j++)
        {
            const float lat = j * dlat;
            const float x = lon_xy * cosf(lat);
            const float y = lon_xy * sinf(lat);
            build.vertices.emplace_back(glm::vec3(x, y, z), color);
        }
    }
    build.vertices.emplace_back(glm::vec3(0.f, 0.f, -1.f), color);

    for (std::uint32_t j = 0; j < lat_partitions; j++)
    {
        build.indices.emplace_back(0);
        build.indices.emplace_back(j + 1);
        build.indices.emplace_back((j + 2) % (lat_partitions + 1));
    }
    for (std::uint32_t i = 0; i < lon_partitions - 3; i++)
        for (std::uint32_t j = 0; j < lat_partitions; j++)
        {
            const std::uint32_t idx1 = lat_partitions * i + j + 1;
            const std::uint32_t idx21 = lat_partitions * (i + 1) + j + 1;
            const std::uint32_t idx12 = lat_partitions * i + (j + 2) % (lat_partitions + 1);
            const std::uint32_t idx2 = lat_partitions * (i + 1) + (j + 2) % (lat_partitions + 1);

            build.indices.emplace_back(idx1);
            build.indices.emplace_back(idx21);
            build.indices.emplace_back(idx2);

            build.indices.emplace_back(idx1);
            build.indices.emplace_back(idx12);
            build.indices.emplace_back(idx2);
        }
    for (std::uint32_t j = 0; j < lat_partitions; j++)
    {
        build.indices.emplace_back(build.vertices.size() - 1);
        build.indices.emplace_back(lat_partitions * (lon_partitions - 3) + j + 1);
        build.indices.emplace_back(lat_partitions * (lon_partitions - 3) + (j + 2) % (lat_partitions + 1));
    }
    return build;
}

model3D::vertex_index_pair model3D::cube(const color &color)
{
    const vertex_index_pair build = {
        {
            {{-.5f, -.5f, -.5f}, color},
            {{-.5f, .5f, .5f}, color},
            {{-.5f, -.5f, .5f}, color},
            {{-.5f, .5f, -.5f}, color},

            {{.5f, -.5f, -.5f}, color},
            {{.5f, .5f, .5f}, color},
            {{.5f, -.5f, .5f}, color},
            {{.5f, .5f, -.5f}, color},

        },
        {0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 0, 6, 2, 0, 4, 6, 3, 5, 1, 3, 7, 5, 2, 5, 1, 2, 6, 5, 0, 7, 3, 0, 4, 7}};
    return build;
}

} // namespace lynx