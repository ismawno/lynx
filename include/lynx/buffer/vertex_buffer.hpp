#pragma once

#include "lynx/buffer/tight_buffer.hpp"
#include "lynx/geometry/vertex.hpp"

namespace lynx
{
template <typename Dim> class vertex_buffer : public tight_buffer<vertex<Dim>>
{
  public:
    using vertex_t = vertex<Dim>;

    vertex_buffer(const kit::ref<const device> &dev, std::size_t size, VkMemoryPropertyFlags properties,
                  VkBufferUsageFlags additional_usage = 0);
};

using vertex_buffer2D = vertex_buffer<dimension::two>;
using vertex_buffer3D = vertex_buffer<dimension::three>;
} // namespace lynx
