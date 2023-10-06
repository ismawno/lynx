#ifndef LYNX_TIGHT_BUFFER_HPP
#define LYNX_TIGHT_BUFFER_HPP

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

    T *map(std::size_t index_offset = 0, std::size_t map_size = SIZE_T_MAX, VkMemoryMapFlags flags = 0);
    bool unmap();

    void flush(std::size_t index_offset = 0, std::size_t flush_size = SIZE_T_MAX);

  private:
    kit::ref<const device> m_device;
    T *m_mapped_data;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    std::size_t m_size;
    VkBufferUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    void cleanup();
};
} // namespace lynx

#endif