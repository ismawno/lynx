#include "lynx/pch.hpp"
#include "lynx/vertex_buffer.hpp"
#include "lynx/device.hpp"
#include "lynx/model.hpp"
#include "lynx/vertex.hpp"

namespace lynx
{
template <typename T>
vertex_buffer::vertex_buffer(const ref<const device> &dev, const std::vector<T> &vertices)
    : m_device(dev), m_has_index_buffer(false)
{
    create_vertex_buffer(vertices);
}

template vertex_buffer::vertex_buffer(const ref<const device> &dev, const std::vector<vertex2D> &vertices);
template vertex_buffer::vertex_buffer(const ref<const device> &dev, const std::vector<vertex3D> &vertices);

template <typename T>
vertex_buffer::vertex_buffer(const ref<const device> &dev, const std::vector<T> &vertices,
                             const std::vector<std::uint32_t> &indices)
    : m_device(dev), m_has_index_buffer(true)
{
    DBG_ASSERT_ERROR(vertices.size() > 1, "Must have at least 2 vertices when using index buffers")
    DBG_ASSERT_ERROR(!indices.empty(), "If specified, indices must not be empty")
    create_vertex_buffer(vertices);
    create_index_buffer(indices);
}

template vertex_buffer::vertex_buffer(const ref<const device> &dev, const std::vector<vertex2D> &vertices,
                                      const std::vector<std::uint32_t> &indices);
template vertex_buffer::vertex_buffer(const ref<const device> &dev, const std::vector<vertex3D> &vertices,
                                      const std::vector<std::uint32_t> &indices);

vertex_buffer::~vertex_buffer()
{
    vkDestroyBuffer(m_device->vulkan_device(), m_vertex_buffer, nullptr);
    vkFreeMemory(m_device->vulkan_device(), m_vertex_buffer_memory, nullptr);
    if (m_has_index_buffer)
    {
        vkDestroyBuffer(m_device->vulkan_device(), m_index_buffer, nullptr);
        vkFreeMemory(m_device->vulkan_device(), m_index_buffer_memory, nullptr);
    }
}

template <typename T>
void vertex_buffer::create_buffer(const std::vector<T> &buffer_data, VkBufferUsageFlags usage, VkBuffer &buffer,
                                  VkDeviceMemory &buffer_memory)
{
    VkDeviceSize buffer_size = sizeof(T) * buffer_data.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    m_device->create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer,
                            staging_memory);
    void *data;
    vkMapMemory(m_device->vulkan_device(), staging_memory, 0, buffer_size, 0, &data);
    memcpy(data, buffer_data.data(), (std::size_t)buffer_size);
    vkUnmapMemory(m_device->vulkan_device(), staging_memory);

    m_device->create_buffer(buffer_size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            buffer, buffer_memory);
    m_device->copy_buffer(buffer, staging_buffer, buffer_size);

    vkDestroyBuffer(m_device->vulkan_device(), staging_buffer, nullptr);
    vkFreeMemory(m_device->vulkan_device(), staging_memory, nullptr);
}

template <typename T> void vertex_buffer::create_vertex_buffer(const std::vector<T> &vertices)
{
    create_buffer(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_vertex_buffer, m_vertex_buffer_memory);
}

void vertex_buffer::create_index_buffer(const std::vector<std::uint32_t> &indices)
{
    create_buffer(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_index_buffer, m_index_buffer_memory);
}

void vertex_buffer::bind(VkCommandBuffer command_buffer) const
{
    const std::array<VkBuffer, 1> buffers = {m_vertex_buffer};
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());
    if (m_has_index_buffer)
        vkCmdBindIndexBuffer(command_buffer, m_index_buffer, 0, VK_INDEX_TYPE_UINT32);
}

bool vertex_buffer::has_index_buffer() const
{
    return m_has_index_buffer;
}

} // namespace lynx