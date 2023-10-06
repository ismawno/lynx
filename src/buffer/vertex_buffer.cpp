#include "lynx/internal/pch.hpp"
#include "lynx/buffer/vertex_buffer.hpp"

namespace lynx
{
template <typename Dim>
vertex_buffer<Dim>::vertex_buffer(const kit::ref<const device> &dev, std::size_t size,
                                  const VkMemoryPropertyFlags properties, const VkBufferUsageFlags additional_usage)
    : tight_buffer<vertex_t>(dev, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | additional_usage, properties)
{
}

template class vertex_buffer<dimension::two>;
template class vertex_buffer<dimension::three>;
} // namespace lynx