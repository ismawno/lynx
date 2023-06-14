#include "lynx/pch.hpp"
#include "lynx/buffer.hpp"
#include "lynx/device.hpp"

namespace lynx
{
buffer::buffer(const ref<const device> &dev, VkDeviceSize instance_size, std::uint32_t instance_count,
               VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize min_offset_alignment)
    : m_device(dev), m_instance_size(instance_size), m_alignment_size(alignment(m_instance_size, min_offset_alignment)),
      m_buffer_size(m_instance_count * m_alignment_size), m_instance_count(instance_count), m_usage(usage),
      m_properties(properties)
{
    m_device->create_buffer(m_buffer_size, usage, properties, m_buffer, m_memory);
}

buffer::~buffer()
{
    unmap();
    vkDestroyBuffer(m_device->vulkan_device(), m_buffer, nullptr);
    vkFreeMemory(m_device->vulkan_device(), m_memory, nullptr);
}

VkResult buffer::map(const VkDeviceSize size, const VkDeviceSize offset)
{
    if (m_mapped_data)
        unmap();
    return vkMapMemory(m_device->vulkan_device(), m_memory, offset, size, 0, &m_mapped_data);
}

bool buffer::unmap()
{
    if (!m_mapped_data)
        return false;
    vkUnmapMemory(m_device->vulkan_device(), m_memory);
    m_memory = nullptr;
    return true;
}

void buffer::write(const void *data, const VkDeviceSize size, const VkDeviceSize offset) const
{
    DBG_ASSERT_ERROR(m_mapped_data, "Cannot copy to unmapped buffer")
    DBG_ASSERT_ERROR((size == VK_WHOLE_SIZE && offset == 0) ||
                         (size != VK_WHOLE_SIZE && (offset + size) < m_buffer_size),
                     "Size + offset must be lower than the buffer size")
    if (size == VK_WHOLE_SIZE)
        memcpy(m_mapped_data, data, m_buffer_size);
    else
    {
        char *offsetted = (char *)m_mapped_data + offset;
        memcpy(offsetted, data, size);
    }
}

void buffer::write(const buffer &src_buffer) const
{
    DBG_ASSERT_ERROR(m_buffer_size >= src_buffer.buffer_size(),
                     "Destination buffer size must be at least equal to the src buffer size")
    m_device->copy_buffer(m_buffer, src_buffer.vulkan_buffer(), m_buffer_size);
}

VkMappedMemoryRange buffer::mapped_memory_range(const VkDeviceSize size, const VkDeviceSize offset) const
{
    VkMappedMemoryRange mapped_range = {};
    mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mapped_range.memory = m_memory;
    mapped_range.offset = offset;
    mapped_range.size = size;
}

VkResult buffer::flush(const VkDeviceSize size, const VkDeviceSize offset) const
{
    const VkMappedMemoryRange mapped_range = mapped_memory_range(size, offset);
    return vkFlushMappedMemoryRanges(m_device->vulkan_device(), 1, &mapped_range);
}

VkDescriptorBufferInfo buffer::descriptor_info(const VkDeviceSize size, const VkDeviceSize offset) const
{
    return VkDescriptorBufferInfo{
        m_buffer,
        offset,
        size,
    };
}

VkResult buffer::invalidate(const VkDeviceSize size, const VkDeviceSize offset) const
{
    const VkMappedMemoryRange mapped_range = mapped_memory_range(size, offset);
    return vkInvalidateMappedMemoryRanges(m_device->vulkan_device(), 1, &mapped_range);
}

void buffer::write_at_index(const void *data, std::uint32_t index) const
{
    write(data, m_instance_size, index * m_alignment_size);
}

VkResult buffer::flush_at_index(std::uint32_t index) const
{
    return flush(m_alignment_size, index * m_alignment_size);
}

VkDescriptorBufferInfo buffer::descriptor_info_at_index(std::uint32_t index) const
{
    return descriptor_info(m_alignment_size, index * m_alignment_size);
}

VkResult buffer::invalidate_at_index(std::uint32_t index) const
{
    return invalidate(m_alignment_size, index * m_alignment_size);
}

VkBuffer buffer::vulkan_buffer() const
{
    return m_buffer;
}
VkDeviceSize buffer::instance_size() const
{
    return m_instance_size;
}
std::uint32_t buffer::instance_count() const
{
    return m_instance_count;
}
VkDeviceSize buffer::buffer_size() const
{
    return m_buffer_size;
}
VkBufferUsageFlags buffer::usage() const
{
    return m_usage;
}
VkMemoryPropertyFlags buffer::properties() const
{
    return m_properties;
}

} // namespace lynx