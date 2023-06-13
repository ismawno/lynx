#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "lynx/core.hpp"
#include "lynx/vertex_buffer.hpp"

namespace lynx
{
struct vertex3D;
struct vertex3D;
class model
{
  public:
    template <typename T> struct vertex_index_pair
    {
        std::vector<T> vertices;
        std::vector<std::uint32_t> indices;
    };

    template <typename T> model(const ref<const device> &dev, const std::vector<T> &vertices);
    template <typename T>
    model(const ref<const device> &dev, const std::vector<T> &vertices, const std::vector<std::uint32_t> &indices);
    template <typename T> model(const ref<const device> &dev, const vertex_index_pair<T> &build);

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

  private:
    vertex_buffer m_vertex_buffer;
    std::size_t m_vertex_count;
    std::size_t m_index_count;
};

class model2D : public model
{
  public:
    using vertex_index_pair = model::vertex_index_pair<vertex2D>;

    model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices);
    model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices,
            const std::vector<std::uint32_t> &indices);
    model2D(const ref<const device> &dev, const vertex_index_pair &build);
    // create NGon
    static const vertex_index_pair &rect(const glm::vec3 &color = glm::vec3(1.f));
    static const std::vector<vertex2D> &line(const glm::vec3 &color = glm::vec3(1.f));
};

class model3D : public model
{
  public:
    using vertex_index_pair = model::vertex_index_pair<vertex3D>;
    model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices);
    model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices,
            const std::vector<std::uint32_t> &indices);
    model3D(const ref<const device> &dev, const vertex_index_pair &build);

    static const vertex_index_pair &rect(const glm::vec3 &color = glm::vec3(1.f));
    static const vertex_index_pair &cube(const glm::vec3 &color = glm::vec3(1.f));
    static const std::vector<vertex3D> &line(const glm::vec3 &color = glm::vec3(1.f));
};
} // namespace lynx

#endif