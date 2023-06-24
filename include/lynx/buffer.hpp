#ifndef LYNX_BUFFER_HPP
#define LYNX_BUFFER_HPP

#include "lynx/core.hpp"
#include "lynx/device.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class buffer
{
  public:
    buffer(const ref<const device> &dev, VkDeviceSize instance_size, std::size_t instance_count,
           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize min_offset_alignment = 1);
    ~buffer();

    void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
    bool unmap();

    void write(const buffer &src_buffer);

    void write(const void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void write_at_index(const void *data, std::size_t index);
    VkResult flush_at_index(std::size_t index);
    VkDescriptorBufferInfo descriptor_info_at_index(std::size_t index) const;
    VkResult invalidate_at_index(std::size_t index);

    template <typename T = void> const T &read_at_index(const std::size_t index) const
    {
        DBG_ASSERT_ERROR(m_mapped_data, "Cannot read from unmapped buffer")
        const char *offsetted = (const char *)m_mapped_data + index * m_alignment_size;
        return *((const T *)offsetted);
    }

    template <typename T = void> T &read_at_index(const std::size_t index)
    {
        DBG_ASSERT_ERROR(m_mapped_data, "Cannot read from unmapped buffer")
        char *offsetted = (char *)m_mapped_data + index * m_alignment_size;
        return *((T *)offsetted);
    }

    VkBuffer vulkan_buffer() const;
    VkDeviceSize instance_size() const;
    std::size_t instance_count() const;
    VkDeviceSize buffer_size() const;
    VkBufferUsageFlags usage() const;
    VkMemoryPropertyFlags properties() const;

  private:
    ref<const device> m_device;
    void *m_mapped_data = nullptr;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    std::size_t m_instance_count;
    VkDeviceSize m_instance_size;
    VkDeviceSize m_alignment_size;
    VkDeviceSize m_buffer_size = 0;

    VkBufferUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    VkMappedMemoryRange mapped_memory_range(VkDeviceSize size, VkDeviceSize offset);

    buffer(const buffer &) = delete;
    void operator=(const buffer &) = delete;
};
} // namespace lynx

#endif