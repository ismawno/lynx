#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "lynx/core.hpp"
#include "lynx/vertex_buffer.hpp"

namespace lynx
{
struct vertex2D;
struct vertex3D;
class model
{
  public:
    model(const ref<const device> &dev, const std::vector<vertex2D> &vertices);
    model(const ref<const device> &dev, const std::vector<vertex3D> &vertices);

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

  private:
    vertex_buffer m_vertex_buffer;
    std::size_t m_vertex_count;
};

class model2D : public model
{
  public:
    model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices);
    // create NGon
    static const std::vector<vertex2D> &rect(const glm::vec3 &color = glm::vec3(1.f));
    static const std::vector<vertex2D> &line(const glm::vec3 &color = glm::vec3(1.f));
};

class model3D : public model
{
  public:
    model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices);
    static const std::vector<vertex3D> &rect(const glm::vec3 &color = glm::vec3(1.f));
    static const std::vector<vertex3D> &cube(const glm::vec3 &color = glm::vec3(1.f));
    static const std::vector<vertex3D> &line(const glm::vec3 &color = glm::vec3(1.f));
};
} // namespace lynx

#endif