#pragma once

#include "kit/memory/ref.hpp"
#include "kit/memory/scope.hpp"
#include "lynx/internal/dimension.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/buffer/vertex_buffer.hpp"
#include "lynx/buffer/index_buffer.hpp"
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

template <Dimension Dim> class model
{
  public:
    using vertex_t = vertex<Dim>;
    using vertex_buffer_t = vertex_buffer<Dim>;
    using vec_t = glm::vec<Dim::N, float>;

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

    const vertex_t *vertex_data() const;
    vertex_t *vertex_data();

    const std::uint32_t *index_data() const;
    std::uint32_t *index_data();

    const vertex_t &vertex(std::size_t index) const;
    void vertex(std::size_t index, const vertex_t &vtx);

    std::uint32_t index(std::size_t index) const;
    void index(std::size_t index, std::uint32_t idx);

    std::size_t vertex_count() const;
    std::size_t index_count() const;

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

  private:
    kit::ref<const device> m_device;

    kit::scope<vertex_buffer_t> m_vertex_buffer;
    kit::scope<index_buffer> m_index_buffer;

    vertex_t *m_vertex_data = nullptr;
    std::uint32_t *m_index_data = nullptr;

    void copy(const model &other);
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
