#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "kit/memory/ref.hpp"
#include "kit/memory/scope.hpp"
#include "lynx/internal/dimension.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/rendering/buffer.hpp"
#include "lynx/geometry/vertex.hpp"

#include <functional>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
class device;

template <typename Dim> class model
{
  public:
    using vertex_t = vertex<Dim>;
    using vec_t = typename Dim::vec_t;

    struct vertex_index_pair
    {
        std::vector<vertex_t> vertices;
        std::vector<std::uint32_t> indices;
    };

    model(const kit::ref<const device> &dev, const std::vector<vertex_t> &vertices);

    model(const kit::ref<const device> &dev, const std::vector<vertex_t> &vertices,
          const std::vector<std::uint32_t> &indices);
    model(const kit::ref<const device> &dev, const vertex_index_pair &build);

    model(const model &other);
    model &operator=(const model &other);

#ifdef DEBUG
    virtual ~model();
#else
    virtual ~model() = default;
#endif

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

    bool has_index_buffers() const;

    void write_vertex(std::size_t buffer_index, const vertex_t &vertex);
    void write_index(std::size_t buffer_index, std::uint32_t index);

    const vertex_t &read_vertex(std::size_t buffer_index) const;
    std::uint32_t read_index(std::size_t buffer_index) const;
    const vertex_t &operator[](std::size_t index) const;

    void update_vertex_buffer(const std::function<void(std::size_t, vertex_t &)> &for_each_fn = nullptr);
    void update_index_buffer(const std::function<void(std::size_t, std::uint32_t &)> &for_each_fn = nullptr);

    std::size_t vertices_count() const;
    std::size_t indices_count() const;

    static vertex_index_pair rect(const color &color);
    static std::vector<vertex_t> line(const color &color1, const color &color2);
    static vertex_index_pair circle(std::uint32_t partitions, const color &color);
    static vertex_index_pair polygon(const std::vector<vertex_t> &local_vertices, const color &center_color);
    static vertex_index_pair polygon(const std::vector<vec_t> &local_vertices, const color &color);

#ifdef DEBUG
    mutable bool to_be_rendered = false;
#endif

  protected:
    model() = default;
    void copy(const model &other);

  private:
    kit::ref<const device> m_device;

    kit::scope<buffer> m_device_vertex_buffer;
    kit::scope<buffer> m_host_vertex_buffer;

    kit::scope<buffer> m_device_index_buffer;
    kit::scope<buffer> m_host_index_buffer;

    void create_vertex_buffer(const std::vector<vertex_t> &vertices);
    void create_index_buffer(const std::vector<std::uint32_t> &indices);
};

using model2D = model<dimension::two>;
class model3D : public model<dimension::three>
{
  public:
    using model::model;

    static vertex_index_pair sphere(std::uint32_t lat_partitions, std::uint32_t lon_partitions, const color &color);
    static vertex_index_pair cube(const color &color);
};
} // namespace lynx

#endif