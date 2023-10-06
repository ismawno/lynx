#include "lynx/internal/pch.hpp"
#include "lynx/buffer/index_buffer.hpp"

namespace lynx
{
index_buffer::index_buffer(const kit::ref<const device> &dev, std::size_t size, VkMemoryPropertyFlags properties,
                           VkBufferUsageFlags additional_usage)
    : tight_buffer<std::uint32_t>(dev, size, properties, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | additional_usage)
{
}
} // namespace lynx