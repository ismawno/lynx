#include "lynx/internal/pch.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/geometry/camera.hpp"

namespace lynx
{
template <Dimension Dim>
model<Dim>::model(const kit::ref<const device> &dev, const std::vector<vertex_t> &vertices) : m_device(dev)
{
    KIT_ASSERT_ERROR(!vertices.empty(), "Cannot create a model with no vertices")
    m_vertex_buffer = kit::make_scope<vertex_buffer_t>(
        m_device, vertices.size(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_vertex_data = m_vertex_buffer->map();
    for (std::size_t i = 0; i < vertices.size(); i++)
        m_vertex_data[i] = vertices[i];
    m_index_buffer = nullptr;
}

template <Dimension Dim>
model<Dim>::model(const kit::ref<const device> &dev, const std::vector<vertex_t> &vertices,
                  const std::vector<std::uint32_t> &indices)
    : m_device(dev)
{
    KIT_ASSERT_ERROR(!vertices.empty(), "Cannot create a model with no vertices")
    KIT_ASSERT_ERROR(!indices.empty(), "If specified, indices must not be empty")
    m_vertex_buffer = kit::make_scope<vertex_buffer_t>(
        m_device, vertices.size(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_vertex_data = m_vertex_buffer->map();
    for (std::size_t i = 0; i < vertices.size(); i++)
        m_vertex_data[i] = vertices[i];

    m_index_buffer = kit::make_scope<index_buffer>(
        m_device, indices.size(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_index_data = m_index_buffer->map();
    for (std::size_t i = 0; i < indices.size(); i++)
        m_index_data[i] = indices[i];
}

template <Dimension Dim>
model<Dim>::model(const kit::ref<const device> &dev, const vertex_index_pair &build)
    : model(dev, build.vertices, build.indices)
{
}

template <Dimension Dim> model<Dim>::model(const model &other)
{
    copy(other);
}

template <Dimension Dim> model<Dim> &model<Dim>::operator=(const model &other)
{
    copy(other);
    return *this;
}

template <Dimension Dim> void model<Dim>::copy(const model &other)
{
    m_device = other.m_device;
    m_vertex_buffer = kit::make_scope<vertex_buffer_t>(*other.m_vertex_buffer);
    m_index_buffer = other.has_index_buffers() ? kit::make_scope<index_buffer>(*other.m_index_buffer) : nullptr;

    m_vertex_data = m_vertex_buffer->data();
    m_index_data = other.has_index_buffers() ? m_index_buffer->data() : nullptr;
}

template <Dimension Dim> void model<Dim>::bind(VkCommandBuffer command_buffer) const
{
    const std::array<VkBuffer, 1> buffers = {m_vertex_buffer->vulkan_buffer()};
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());

    if (m_index_buffer)
        vkCmdBindIndexBuffer(command_buffer, m_index_buffer->vulkan_buffer(), 0, VK_INDEX_TYPE_UINT32);
}
template <Dimension Dim> void model<Dim>::draw(VkCommandBuffer command_buffer) const
{
    if (has_index_buffers())
        vkCmdDrawIndexed(command_buffer, (std::uint32_t)m_index_buffer->size(), 1, 0, 0, 0);
    else
        vkCmdDraw(command_buffer, (std::uint32_t)m_vertex_buffer->size(), 1, 0, 0);
}

template <Dimension Dim> bool model<Dim>::has_index_buffers() const
{
    return m_index_data && m_index_buffer;
}

template <Dimension Dim> const vertex<Dim> *model<Dim>::vertex_data() const
{
    return m_vertex_data;
}
template <Dimension Dim> vertex<Dim> *model<Dim>::vertex_data()
{
    return m_vertex_data;
}

template <Dimension Dim> const std::uint32_t *model<Dim>::index_data() const
{
    KIT_ASSERT_ERROR(has_index_buffers(), "Current model does not contain an index buffer!")
    return m_index_data;
}
template <Dimension Dim> std::uint32_t *model<Dim>::index_data()
{
    KIT_ASSERT_ERROR(has_index_buffers(), "Current model does not contain an index buffer!")
    return m_index_data;
}

template <Dimension Dim> const vertex<Dim> &model<Dim>::vertex(const std::size_t index) const
{
    return m_vertex_data[index];
}
template <Dimension Dim> void model<Dim>::vertex(const std::size_t index, const vertex_t &vtx)
{
    m_vertex_data[index] = vtx;
}

template <Dimension Dim> std::uint32_t model<Dim>::index(const std::size_t index) const
{
    return m_index_data[index];
}
template <Dimension Dim> void model<Dim>::index(const std::size_t index, const std::uint32_t idx)
{
    m_index_data[index] = idx;
}

template <Dimension Dim> std::size_t model<Dim>::vertex_count() const
{
    return m_vertex_buffer->size();
}

template <Dimension Dim> std::size_t model<Dim>::index_count() const
{
    return m_index_buffer ? m_index_buffer->size() : 0;
}

template <Dimension Dim> typename model<Dim>::vertex_index_pair model<Dim>::rect(const color &color)
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

template <Dimension Dim> std::vector<vertex<Dim>> model<Dim>::line(const color &color1, const color &color2)
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
        return {{{-1.f, 0.f}, color1}, {{1.f, 0.f}, color2}};
    else
        return {{{-1.f, 0.f, 0.f}, color1}, {{1.f, 0.f, 0.f}, color2}};
}
template <Dimension Dim>
typename model<Dim>::vertex_index_pair model<Dim>::circle(const std::uint32_t partitions, const color &color)
{
    KIT_ASSERT_ERROR(partitions > 2, "Must at least have 3 partitions. Current: {0}", partitions)

    vertex_index_pair build;
    build.vertices.emplace_back(vec_t(0.f), color);
    build.indices.resize(3 * partitions);
    build.vertices.reserve(partitions);

    const float dangle = 2.f * glm::pi<float>() / (partitions - 1);
    for (std::uint32_t i = 0; i < partitions; i++)
    {
        const float angle = i * dangle;
        if constexpr (std::is_same_v<vec_t, glm::vec2>)
            build.vertices.emplace_back(vec_t(cosf(angle), sinf(angle)), color);
        else
            build.vertices.emplace_back(vec_t(cosf(angle), sinf(angle), 0.f), color);
        build.indices[3 * i] = 0;
        build.indices[3 * i + 1] = i + 1;
        build.indices[3 * i + 2] = i + 2;
    }
    return build;
}
template <Dimension Dim>
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
template <Dimension Dim>
typename model<Dim>::vertex_index_pair model<Dim>::polygon(const std::vector<vec_t> &local_vertices, const color &color)
{
    std::vector<vertex_t> colored_vertices;
    colored_vertices.reserve(local_vertices.size());

    for (const vec_t &v : local_vertices)
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

template class model<dimension::two>;
template class model<dimension::three>;

} // namespace lynx