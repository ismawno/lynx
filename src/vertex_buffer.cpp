#include "lynx/pch.hpp"
#include "lynx/vertex_buffer.hpp"
#include "lynx/device.hpp"
#include "lynx/model.hpp"

namespace lynx
{
vertex_buffer::vertex_buffer(const device &dev, const std::vector<vertex2D> &vertices) : m_device(dev)
{
    create_vertex_buffers(vertices);
}

vertex_buffer::vertex_buffer(const device &dev, const std::vector<vertex3D> &vertices) : m_device(dev)
{
    create_vertex_buffers(vertices);
}

vertex_buffer::~vertex_buffer()
{
    vkDestroyBuffer(m_device.vulkan_device(), m_vertex_buffer, nullptr);
    vkFreeMemory(m_device.vulkan_device(), m_vertex_buffer_memory, nullptr);
}

template <typename T> void vertex_buffer::create_vertex_buffers(const std::vector<T> &vertices)
{
    VkDeviceSize buffer_size = sizeof(T) * vertices.size();
    m_device.create_buffer(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vertex_buffer,
                           m_vertex_buffer_memory);
    void *data;
    vkMapMemory(m_device.vulkan_device(), m_vertex_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), (std::size_t)buffer_size);
    vkUnmapMemory(m_device.vulkan_device(), m_vertex_buffer_memory);
}

void vertex_buffer::bind(VkCommandBuffer command_buffer) const
{
    const std::array<VkBuffer, 1> buffers = {m_vertex_buffer};
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());
}

} // namespace lynx