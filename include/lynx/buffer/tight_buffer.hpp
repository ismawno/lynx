#pragma once

#include "kit/memory/ref.hpp"

#include "lynx/internal/dimension.hpp"
#include "lynx/rendering/device.hpp"

namespace lynx
{
template <typename T> class tight_buffer
{
  public:
    tight_buffer(const kit::ref<const device> &dev, std::size_t size, VkMemoryPropertyFlags properties,
                 VkBufferUsageFlags usage);
    ~tight_buffer();

    tight_buffer(const tight_buffer &other);
    tight_buffer &operator=(const tight_buffer &other);

    T *map(std::size_t index_offset = 0, std::size_t map_size = SIZE_MAX, VkMemoryMapFlags flags = 0);
    bool unmap();

    const T *data() const;
    T *data();

    void flush(std::size_t index_offset = 0, std::size_t flush_size = SIZE_MAX);
    void transfer(const tight_buffer &src_buffer);

    VkBuffer vulkan_buffer() const;
    std::size_t size() const;
    bool mapped() const;

  private:
    kit::ref<const device> m_device;
    T *m_mapped_data = nullptr;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    std::size_t m_size;
    VkBufferUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    void cleanup();
};
} // namespace lynx
