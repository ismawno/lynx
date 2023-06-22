#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "lynx/core.hpp"
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>

namespace lynx
{
struct vertex2D;
struct vertex3D;

class buffer;
class device;

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

    virtual ~model() = default;

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

  private:
    scope<buffer> m_device_vertex_buffer;
    scope<buffer> m_host_vertex_buffer;

    scope<buffer> m_device_index_buffer;
    scope<buffer> m_host_index_buffer;

    template <typename T> void create_vertex_buffer(const ref<const device> &dev, const std::vector<T> &vertices);
    void create_index_buffer(const ref<const device> &dev, const std::vector<std::uint32_t> &indices);
};

class model2D : public model
{
  public:
    using vertex_index_pair = model::vertex_index_pair<vertex2D>;

    model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices);
    model2D(const ref<const device> &dev, const std::vector<vertex2D> &vertices,
            const std::vector<std::uint32_t> &indices);
    model2D(const ref<const device> &dev, const vertex_index_pair &build);
    // create NGon. HACER STATIC UNORDERED MAP
    static const vertex_index_pair &rect(const glm::vec4 &color = glm::vec4(1.f));
    static const std::vector<vertex2D> &line(const glm::vec4 &color = glm::vec4(1.f));
};

class model3D : public model
{
  public:
    using vertex_index_pair = model::vertex_index_pair<vertex3D>;
    model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices);
    model3D(const ref<const device> &dev, const std::vector<vertex3D> &vertices,
            const std::vector<std::uint32_t> &indices);
    model3D(const ref<const device> &dev, const vertex_index_pair &build);

    static const vertex_index_pair &rect(const glm::vec4 &color = glm::vec4(1.f));
    static const vertex_index_pair &cube(const glm::vec4 &color = glm::vec4(1.f));
    static const std::vector<vertex3D> &line(const glm::vec4 &color = glm::vec4(1.f));
};
} // namespace lynx

#endif