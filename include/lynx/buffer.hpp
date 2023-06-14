#ifndef LYNX_BUFFER_HPP
#define LYNX_BUFFER_HPP

#include "lynx/core.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
class buffer
{
  public:
    buffer(const ref<const device> &dev, VkDeviceSize instance_size, std::uint32_t instance_count,
           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize min_offset_alignment = 1);
    ~buffer();

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    bool unmap();

    void write(const buffer &src_buffer) const;

    void write(const void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
    VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;

    void write_at_index(const void *data, std::uint32_t index) const;
    VkResult flush_at_index(std::uint32_t index) const;
    VkDescriptorBufferInfo descriptor_info_at_index(std::uint32_t index) const;
    VkResult invalidate_at_index(std::uint32_t index) const;

    VkBuffer vulkan_buffer() const;
    VkDeviceSize instance_size() const;
    std::uint32_t instance_count() const;
    VkDeviceSize buffer_size() const;
    VkBufferUsageFlags usage() const;
    VkMemoryPropertyFlags properties() const;

  private:
    ref<const device> m_device;
    void *m_mapped_data = nullptr;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    VkDeviceSize m_instance_size;
    VkDeviceSize m_alignment_size;
    VkDeviceSize m_buffer_size = 0;

    std::uint32_t m_instance_count;
    VkBufferUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    VkMappedMemoryRange mapped_memory_range(VkDeviceSize size, VkDeviceSize offset) const;
    static VkDeviceSize alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);

    buffer(const buffer &) = delete;
    void operator=(const buffer &) = delete;
};
} // namespace lynx

#endif