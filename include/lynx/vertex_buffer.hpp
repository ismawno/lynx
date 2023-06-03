#ifndef LYNX_VERTEX_BUFFER_HPP
#define LYNX_VERTEX_BUFFER_HPP

#include "lynx/core.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;

class vertex_buffer
{
  public:
    template <typename T> vertex_buffer(const device &dev, const std::vector<T> &vertices);
    virtual ~vertex_buffer();

    void bind(VkCommandBuffer command_buffer) const;

  private:
    const device &m_device;
    VkBuffer m_vertex_buffer;
    VkDeviceMemory m_vertex_buffer_memory;

    template <typename T> void create_vertex_buffers(const std::vector<T> &vertices);

    vertex_buffer(const vertex_buffer &) = delete;
    void operator=(const vertex_buffer &) = delete;
};
} // namespace lynx

#endif