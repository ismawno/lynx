#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "lynx/device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace lynx
{
class model
{
  public:
    struct vertex
    {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
    };

    model(const device &dev, const std::vector<vertex> &vertices);
    ~model();

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer);

  private:
    const device &m_device;
    VkBuffer m_vertex_buffer;
    VkDeviceMemory m_vertex_buffer_memory;
    std::size_t m_vertex_count;

    void create_vertex_buffers(const std::vector<vertex> &vertices);

    model(const model &) = delete;
    void operator=(const model &) = delete;
};
} // namespace lynx

#endif