#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "kit/memory/ref.hpp"
#include "kit/memory/scope.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/rendering/buffer.hpp"

#include <functional>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
struct vertex2D;
struct vertex3D;

class device;

class model
{
  public:
    template <typename T> struct vertex_index_pair
    {
        std::vector<T> vertices;
        std::vector<std::uint32_t> indices;
    };

    template <typename T> model(const kit::ref<const device> &dev, const std::vector<T> &vertices);
    template <typename T>
    model(const kit::ref<const device> &dev, const std::vector<T> &vertices, const std::vector<std::uint32_t> &indices);
    template <typename T> model(const kit::ref<const device> &dev, const vertex_index_pair<T> &build);

#ifdef DEBUG
    virtual ~model();
#else
    virtual ~model() = default;
#endif

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

    bool has_index_buffers() const;

    template <typename T> void write_vertex(std::size_t buffer_index, const T &vertex);
    void write_index(std::size_t buffer_index, std::uint32_t index);

    template <typename T> const T &read_vertex(std::size_t buffer_index) const;
    std::uint32_t read_index(std::size_t buffer_index) const;

    template <typename T> void update_vertex_buffer(const std::function<void(T &)> &for_each_fn = nullptr);
    void update_index_buffer(const std::function<void(std::uint32_t &)> &for_each_fn = nullptr);

    std::size_t vertices_count() const;
    std::size_t indices_count() const;

#ifdef DEBUG
    mutable bool to_be_rendered = false;
#endif

  protected:
    model() = default;
    template <typename T> void copy(const model &other);

  private:
    kit::ref<const device> m_device;

    kit::scope<buffer> m_device_vertex_buffer;
    kit::scope<buffer> m_host_vertex_buffer;

    kit::scope<buffer> m_device_index_buffer;
    kit::scope<buffer> m_host_index_buffer;

    template <typename T> void create_vertex_buffer(const std::vector<T> &vertices);
    void create_index_buffer(const std::vector<std::uint32_t> &indices);
};

class model2D : public model
{
  public:
    using vertex_index_pair = model::vertex_index_pair<vertex2D>;

    model2D(const kit::ref<const device> &dev, const std::vector<vertex2D> &vertices);
    model2D(const kit::ref<const device> &dev, const std::vector<vertex2D> &vertices,
            const std::vector<std::uint32_t> &indices);
    model2D(const kit::ref<const device> &dev, const vertex_index_pair &build);

    model2D(const model2D &other);
    model2D &operator=(const model2D &other);

    void write_vertex(std::size_t buffer_index, const vertex2D &vertex);
    const vertex2D &read_vertex(std::size_t buffer_index) const;

    void update_vertex_buffer(const std::function<void(vertex2D &)> &for_each_fn = nullptr);

    const vertex2D &operator[](std::size_t index) const;

    // create NGon. HACER STATIC UNORDERED MAP
    static const vertex_index_pair &rect(const color &color);
    static const std::vector<vertex2D> &line(const color &color1, const color &color2);
    static vertex_index_pair circle(std::uint32_t partitions, const color &color);
    static vertex_index_pair polygon(const std::vector<vertex2D> &local_vertices, const color &center_color);
    static vertex_index_pair polygon(const std::vector<glm::vec2> &local_vertices, const color &color);
};

class model3D : public model
{
  public:
    using vertex_index_pair = model::vertex_index_pair<vertex3D>;
    model3D(const kit::ref<const device> &dev, const std::vector<vertex3D> &vertices);
    model3D(const kit::ref<const device> &dev, const std::vector<vertex3D> &vertices,
            const std::vector<std::uint32_t> &indices);
    model3D(const kit::ref<const device> &dev, const vertex_index_pair &build);

    model3D(const model3D &other);
    model3D &operator=(const model3D &other);

    void write_vertex(std::size_t buffer_index, const vertex3D &vertex);
    const vertex3D &read_vertex(std::size_t buffer_index) const;

    void update_vertex_buffer(const std::function<void(vertex3D &)> &for_each_fn = nullptr);

    const vertex3D &operator[](std::size_t index) const;

    static const vertex_index_pair &rect(const color &color);
    static vertex_index_pair circle(std::uint32_t partitions, const color &color);
    static vertex_index_pair polygon(const std::vector<vertex3D> &local_vertices, const color &center_color);
    static vertex_index_pair polygon(const std::vector<glm::vec3> &local_vertices, const color &color);
    static vertex_index_pair sphere(std::uint32_t lat_partitions, std::uint32_t lon_partitions, const color &color);
    static const vertex_index_pair &cube(const color &color);
    static const std::vector<vertex3D> &line(const color &color1, const color &color2);
};
} // namespace lynx

#endif