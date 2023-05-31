#include "lynx/pch.hpp"
#include "lynx/model.hpp"

namespace lynx
{
model::model(const device &dev, const std::vector<vertex> &vertices) : m_device(dev), m_vertex_count(vertices.size())
{
    DBG_ASSERT_ERROR(m_vertex_count >= 3, "Amount of vertices must be at least 3. Current amount: {0}", m_vertex_count)
    create_vertex_buffers(vertices);
}

model::~model()
{
    vkDestroyBuffer(m_device.vulkan_device(), m_vertex_buffer, nullptr);
    vkFreeMemory(m_device.vulkan_device(), m_vertex_buffer_memory, nullptr);
}

void model::create_vertex_buffers(const std::vector<vertex> &vertices)
{
    VkDeviceSize buffer_size = sizeof(vertex) * m_vertex_count;
    m_device.create_buffer(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vertex_buffer,
                           m_vertex_buffer_memory);
    void *data;
    vkMapMemory(m_device.vulkan_device(), m_vertex_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), (std::size_t)buffer_size);
    vkUnmapMemory(m_device.vulkan_device(), m_vertex_buffer_memory);
}

void model::bind(VkCommandBuffer command_buffer)
{
    const std::array<VkBuffer, 1> buffers = {m_vertex_buffer};
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());
}
void model::draw(VkCommandBuffer command_buffer)
{
    vkCmdDraw(command_buffer, (std::uint32_t)m_vertex_count, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> model::vertex::binding_descriptions()
{
    return {{0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}
std::vector<VkVertexInputAttributeDescription> model::vertex::attribute_descriptions()
{
    return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, color)}};
}

} // namespace lynx