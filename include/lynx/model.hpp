#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "lynx/core.hpp"
#include "lynx/vertex_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace lynx
{
class model
{
  public:
    template <typename T> model(const device &dev, const std::vector<T> &vertices);

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

  private:
    vertex_buffer m_vertex_buffer;
    std::size_t m_vertex_count;
};

class model2D : public model
{
  public:
    struct vertex
    {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
    };

    model2D(const device &dev, const std::vector<vertex> &vertices);
};

class model3D : public model
{
  public:
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
    };

    model3D(const device &dev, const std::vector<vertex> &vertices);
};
} // namespace lynx

#endif