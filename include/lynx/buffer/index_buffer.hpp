#ifndef LYNX_INDEX_BUFFER_HPP
#define LYNX_INDEX_BUFFER_HPP

#include "lynx/buffer/tight_buffer.hpp"

namespace lynx
{
class index_buffer : public tight_buffer<std::uint32_t>
{
  public:
    index_buffer(const kit::ref<const device> &dev, std::size_t size, VkMemoryPropertyFlags properties,
                 VkBufferUsageFlags additional_usage = 0);
};
} // namespace lynx

#endif