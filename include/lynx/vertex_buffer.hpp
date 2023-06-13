#ifndef LYNX_VERTEX_BUFFER_HPP
#define LYNX_VERTEX_BUFFER_HPP

#include "lynx/core.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
struct vertex2D;
struct vertex3D;
class vertex_buffer
{
  public:
    template <typename T> vertex_buffer(const ref<const device> &dev, const std::vector<T> &vertices);
    template <typename T>
    vertex_buffer(const ref<const device> &dev, const std::vector<T> &vertices,
                  const std::vector<std::uint32_t> &indices);

    virtual ~vertex_buffer();

    void bind(VkCommandBuffer command_buffer) const;
    bool has_index_buffer() const;

  private:
    ref<const device> m_device;
    VkBuffer m_vertex_buffer;
    VkDeviceMemory m_vertex_buffer_memory;

    bool m_has_index_buffer;

    VkBuffer m_index_buffer;
    VkDeviceMemory m_index_buffer_memory;

    template <typename T>
    void create_buffer(const std::vector<T> &buffer_data, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                       VkBuffer &buffer, VkDeviceMemory &buffer_memory);
    template <typename T> void create_vertex_buffer(const std::vector<T> &vertices);
    void create_index_buffer(const std::vector<std::uint32_t> &indices);

    vertex_buffer(const vertex_buffer &) = delete;
    void operator=(const vertex_buffer &) = delete;
};
} // namespace lynx

#endif