#include "lynx/internal/pch.hpp"
#include "lynx/buffer/tight_buffer.hpp"
#include "lynx/geometry/vertex.hpp"

namespace lynx
{
template <typename T>
tight_buffer<T>::tight_buffer(const kit::ref<const device> &dev, std::size_t size,
                              const VkMemoryPropertyFlags properties, const VkBufferUsageFlags usage)
    : m_device(dev), m_size(size), m_usage(usage), m_properties(properties)
{
    m_device->create_buffer(size * sizeof(T), usage, properties, m_buffer, m_memory);
}

template <typename T> tight_buffer<T>::~tight_buffer()
{
    cleanup();
}

template <typename T>
tight_buffer<T>::tight_buffer(const tight_buffer &other)
    : m_device(other.m_device), m_size(other.m_size), m_usage(other.m_usage), m_properties(other.m_properties)
{
    m_device->create_buffer(m_size * sizeof(T), m_usage, m_properties, m_buffer, m_memory);
    map();

    const T *other_data = other.data();
    for (std::size_t i = 0; i < m_size; i++)
        m_mapped_data[i] = other_data[i];
}

template <typename T> tight_buffer<T> &tight_buffer<T>::operator=(const tight_buffer &other)
{
    cleanup();

    m_device = other.m_device;
    m_size = other.m_size;
    m_usage = other.m_usage;
    m_properties = other.m_properties;

    m_device->create_buffer(m_size * sizeof(T), m_usage, m_properties, m_buffer, m_memory);
    map();

    const T *other_data = other.data();
    for (std::size_t i = 0; i < m_size; i++)
        m_mapped_data[i] = other_data[i];

    return *this;
}

template <typename T> T *tight_buffer<T>::map(std::size_t index_offset, std::size_t map_size, VkMemoryMapFlags flags)
{
    KIT_ASSERT_ERROR(m_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                     "Buffer memory must be host visible for it to be mapped to cpu memory")
    if (m_mapped_data)
        unmap();

    const VkDeviceSize size = map_size == SIZE_T_MAX ? VK_WHOLE_SIZE : (map_size * sizeof(T));
    KIT_CHECK_RETURN_VALUE(
        vkMapMemory(m_device->vulkan_device(), m_memory, index_offset, size, flags, (void **)&m_mapped_data),
        VK_SUCCESS, CRITICAL, "Failed to map memory");
    return m_mapped_data;
}

template <typename T> bool tight_buffer<T>::unmap()
{
    if (!m_mapped_data)
        return false;
    vkUnmapMemory(m_device->vulkan_device(), m_memory);
    m_mapped_data = nullptr;

    return true;
}

template <typename T> const T *tight_buffer<T>::data() const
{
    KIT_ASSERT_ERROR(m_mapped_data, "Buffer must be mapped to access its data!")
    return m_mapped_data;
}

template <typename T> T *tight_buffer<T>::data()
{
    KIT_ASSERT_ERROR(m_mapped_data, "Buffer must be mapped to access its data!")
    return m_mapped_data;
}

template <typename T> void tight_buffer<T>::flush(std::size_t index_offset, std::size_t flush_size)
{
    const VkDeviceSize size = flush_size == SIZE_T_MAX ? VK_WHOLE_SIZE : (flush_size * sizeof(T));

    VkMappedMemoryRange mapped_range{};
    mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mapped_range.memory = m_memory;
    mapped_range.offset = index_offset;
    mapped_range.size = size;

    KIT_CHECK_RETURN_VALUE(vkFlushMappedMemoryRanges(m_device->vulkan_device(), 1, &mapped_range), VK_SUCCESS, CRITICAL,
                           "Failed to flush memory. size: {0}, offset: {1}", flush_size, index_offset)
}

template <typename T> void tight_buffer<T>::transfer(const tight_buffer &src_buffer)
{
    KIT_ASSERT_ERROR(m_size >= src_buffer.m_size,
                     "Destination buffer size must be at least equal to the src buffer size")
    KIT_ASSERT_ERROR(m_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     "Destintaion buffer must have the VK_BUFFER_USAGE_TRANSFER_DST_BIT flag enabled")
    KIT_ASSERT_ERROR(src_buffer.m_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     "Source buffer must have the VK_BUFFER_USAGE_TRANSFER_SRC_BIT flag enabled")

    m_device->copy_buffer(m_buffer, src_buffer.m_buffer, src_buffer.m_size * sizeof(T));
}

template <typename T> VkBuffer tight_buffer<T>::vulkan_buffer() const
{
    return m_buffer;
}

template <typename T> std::size_t tight_buffer<T>::size() const
{
    return m_size;
}
template <typename T> bool tight_buffer<T>::mapped() const
{
    return m_mapped_data != nullptr;
}

template <typename T> void tight_buffer<T>::cleanup()
{
    vkDeviceWaitIdle(m_device->vulkan_device());
    unmap();
    vkDestroyBuffer(m_device->vulkan_device(), m_buffer, nullptr);
    vkFreeMemory(m_device->vulkan_device(), m_memory, nullptr);
}

template class tight_buffer<vertex2D>;
template class tight_buffer<vertex3D>;
template class tight_buffer<std::uint32_t>;
} // namespace lynx