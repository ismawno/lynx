#ifndef LYNX_BUFFER_HPP
#define LYNX_BUFFER_HPP

#include "kit/memory/ref.hpp"

#include "lynx/utility/non_copyable.hpp"
#include "lynx/rendering/device.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class buffer : non_copyable
{
  public:
    buffer(const kit::ref<const device> &dev, VkDeviceSize instance_size, std::size_t instance_count,
           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize min_offset_alignment = 1);
    ~buffer();

    void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
    bool unmap();

    void write(const buffer &src_buffer);

    void write(const void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
    void invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void write_at_index(const void *data, std::size_t index);
    void flush_at_index(std::size_t index);
    VkDescriptorBufferInfo descriptor_info_at_index(std::size_t index) const;
    void invalidate_at_index(std::size_t index);

    template <typename T> const T &read_at_index(const std::size_t index) const
    {
        DBG_ASSERT_ERROR(m_mapped_data, "Cannot read from unmapped buffer")
        const char *offsetted = (const char *)m_mapped_data + index * m_alignment_size;
        return *((const T *)offsetted);
    }

    template <typename T> T &read_at_index(const std::size_t index)
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
    kit::ref<const device> m_device;
    void *m_mapped_data = nullptr;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    std::size_t m_instance_count;
    VkDeviceSize m_instance_size;
    VkDeviceSize m_min_offset_alignment;
    VkDeviceSize m_alignment_size;
    VkDeviceSize m_buffer_size = 0;

    VkBufferUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    VkMappedMemoryRange mapped_memory_range(VkDeviceSize size, VkDeviceSize offset);
};
} // namespace lynx

#endif