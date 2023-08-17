#include "lynx/internal/pch.hpp"
#include "lynx/utility/context.hpp"
#include "lynx/drawing/shape.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/buffer.hpp"
#include "lynx/geometry/vertex.hpp"
#include "kit/utility/utils.hpp"

namespace lynx
{
// Color should already be encoded in arguments when constructing the model
template <class... ModelArgs>
shape2D::shape2D(topology tplg, ModelArgs &&...args)
    : m_model(context::current()->device(), std::forward<ModelArgs>(args)...),
      m_outline_model(context::current()->device(), std::forward<ModelArgs>(args)...), m_topology(tplg)
{
}

const color &shape2D::color() const
{
    return m_model.read_vertex(0).color;
}
void shape2D::color(const lynx::color &color)
{
    const auto feach = [&color](std::size_t, vertex2D &vtx) { vtx.color = color; };
    m_model.update_vertex_buffer(feach);
}

const color &shape2D::outline_color() const
{
    return m_outline_model.read_vertex(0).color;
}
void shape2D::outline_color(const lynx::color &color)
{
    const auto feach = [&color](std::size_t, vertex2D &vtx) { vtx.color = color; };
    m_outline_model.update_vertex_buffer(feach);
}

float shape2D::outline_thickness() const
{
    return m_outline_thickness;
}
void shape2D::outline_thickness(const float thickness)
{
    m_outline_thickness = thickness;
}

void shape2D::draw(window2D &win) const
{
    if (!kit::approaches_zero(m_outline_thickness))
    {
        const auto feach = [this](std::size_t index, vertex2D &outline_vertex) {
            const glm::vec2 real_vertex = m_model.read_vertex(index).position;
            if (!kit::approaches_zero(glm::length2(real_vertex)) &&
                !kit::approaches_zero(glm::length2(transform.scale)))
                outline_vertex.position =
                    real_vertex + (glm::normalize(real_vertex) * m_outline_thickness) / transform.scale;
        };
        m_outline_model.update_vertex_buffer(feach);
        drawable::default_draw(win, &m_outline_model, transform.center_scale_rotate_translate4(), m_topology);
    }

    drawable::default_draw(win, &m_model, transform.center_scale_rotate_translate4(), m_topology);
}

// Color should already be encoded in arguments when constructing the model
template <class... ModelArgs>
shape3D::shape3D(topology tplg, ModelArgs &&...args)
    : m_model(context::current()->device(), std::forward<ModelArgs>(args)...), m_topology(tplg)
{
}

const color &shape3D::color() const
{
    return m_model.read_vertex(0).color;
}

void shape3D::color(const lynx::color &color)
{
    const auto feach = [&color](std::size_t, vertex3D &vtx) { vtx.color = color; };
    m_model.update_vertex_buffer(feach);
}

void shape3D::draw(window3D &win) const
{
    drawable::default_draw(win, &m_model, transform.center_scale_rotate_translate4(), m_topology);
}

rect2D::rect2D(const glm::vec2 &position, const glm::vec2 &dimensions, const lynx::color &color)
    : shape2D(topology::TRIANGLE_LIST, model2D::rect(color))
{
    transform.position = position;
    transform.scale = dimensions;
}

rect2D::rect2D(const lynx::color &color) : shape2D(topology::TRIANGLE_LIST, model2D::rect(color))
{
}

ellipse2D::ellipse2D(const float ra, const float rb, const lynx::color &color, const std::uint32_t partitions)
    : shape2D(topology::TRIANGLE_LIST, model2D::circle(partitions, color))
{
    transform.scale = {ra, rb};
}
ellipse2D::ellipse2D(const float radius, const lynx::color &color, const std::uint32_t partitions)
    : shape2D(topology::TRIANGLE_LIST, model2D::circle(partitions, color))
{
    transform.scale = {radius, radius};
}
ellipse2D::ellipse2D(const lynx::color &color, const std::uint32_t partitions)
    : shape2D(topology::TRIANGLE_LIST, model2D::circle(partitions, color))
{
}

float ellipse2D::radius() const
{
    return 0.5f * (transform.scale.x + transform.scale.y);
}

void ellipse2D::radius(const float radius)
{
    transform.scale = {radius, radius};
}

polygon2D::polygon2D(const std::vector<glm::vec2> &local_vertices, const lynx::color &color)
    : shape2D(topology::TRIANGLE_LIST, model2D::polygon(local_vertices, color)), m_size(local_vertices.size())
{
}
polygon2D::polygon2D(const std::vector<vertex2D> &local_vertices, const lynx::color &center_color)
    : shape2D(topology::TRIANGLE_LIST, model2D::polygon(local_vertices, center_color)), m_size(local_vertices.size())
{
}

const vertex2D &polygon2D::operator[](const std::size_t index) const
{
    return vertex(index);
}

const vertex2D &polygon2D::vertex(const std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    return m_model.read_vertex(index + 1); // +1 to account for center vertex
}

void polygon2D::vertex(std::size_t index, const vertex2D &vertex)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    m_model.write_vertex(index + 1, vertex);
}
void polygon2D::vertex(const std::size_t index, const glm::vec2 &vertex)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    vertex2D v = m_model.read_vertex(index + 1);
    v.position = vertex;
    m_model.write_vertex(index + 1, v); //+1 to account for center vertex
}

void polygon2D::update_vertices(const std::function<void(std::size_t, vertex2D &)> &for_each_fn)
{
    m_model.update_vertex_buffer(for_each_fn);
}

const lynx::color &polygon2D::color(std::size_t index) const
{
    return m_model.read_vertex(index).color;
}

void polygon2D::color(const lynx::color &color)
{
    shape2D::color(color);
}

void polygon2D::color(std::size_t index, const lynx::color &color)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    vertex2D v = m_model.read_vertex(index + 1);
    v.color = color;
    m_model.write_vertex(index + 1, v);
}

std::size_t polygon2D::size() const
{
    return m_size;
}

rect3D::rect3D(const glm::vec3 &position, const glm::vec2 &dimensions, const lynx::color &color)
    : shape3D(topology::TRIANGLE_LIST, model3D::rect(color))
{
    transform.position = position;
    transform.scale = glm::vec3(dimensions, 1.f);
}

rect3D::rect3D(const lynx::color &color) : shape3D(topology::TRIANGLE_LIST, model3D::rect(color))
{
}

ellipse3D::ellipse3D(const float ra, const float rb, const lynx::color &color, const std::uint32_t partitions)
    : shape3D(topology::TRIANGLE_LIST, model3D::circle(partitions, color))
{
    transform.scale = {ra, rb, 1.f};
}
ellipse3D::ellipse3D(const float radius, const lynx::color &color, const std::uint32_t partitions)
    : shape3D(topology::TRIANGLE_LIST, model3D::circle(partitions, color))
{
    transform.scale = {radius, radius, radius};
}
ellipse3D::ellipse3D(const lynx::color &color, const std::uint32_t partitions)
    : shape3D(topology::TRIANGLE_LIST, model3D::circle(partitions, color))
{
}

float ellipse3D::radius() const
{
    return (transform.scale.x + transform.scale.y + transform.scale.z) / 3.f;
}

void ellipse3D::radius(const float radius)
{
    transform.scale = {radius, radius, radius};
}

polygon3D::polygon3D(const std::vector<glm::vec3> &local_vertices, const lynx::color &color)
    : shape3D(topology::TRIANGLE_LIST, model3D::polygon(local_vertices, color)), m_size(local_vertices.size())
{
}

polygon3D::polygon3D(const std::vector<vertex3D> &local_vertices, const lynx::color &center_color)
    : shape3D(topology::TRIANGLE_LIST, model3D::polygon(local_vertices, center_color)), m_size(local_vertices.size())
{
}

const vertex3D &polygon3D::operator[](const std::size_t index) const
{
    return vertex(index);
}

const vertex3D &polygon3D::vertex(const std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    return m_model.read_vertex(index + 1); // +1 to account for center vertex
}

void polygon3D::vertex(std::size_t index, const vertex3D &vertex)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    m_model.write_vertex(index + 1, vertex);
}
void polygon3D::vertex(const std::size_t index, const glm::vec3 &vertex)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    vertex3D v = m_model.read_vertex(index + 1);
    v.position = vertex;
    m_model.write_vertex(index + 1, v); //+1 to account for center vertex
}

void polygon3D::update_vertices(const std::function<void(std::size_t, vertex3D &)> &for_each_fn)
{
    m_model.update_vertex_buffer(for_each_fn);
}

const lynx::color &polygon3D::color(std::size_t index) const
{
    return m_model.read_vertex(index).color;
}

void polygon3D::color(const lynx::color &color)
{
    shape3D::color(color);
}

void polygon3D::color(std::size_t index, const lynx::color &color)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertices_count() - 1)
    vertex3D v = m_model.read_vertex(index + 1);
    v.color = color;
    m_model.write_vertex(index + 1, v);
}

std::size_t polygon3D::size() const
{
    return m_size;
}

ellipsoid3D::ellipsoid3D(const float ra, const float rb, float rc, const lynx::color &color,
                         const std::uint32_t lat_partitions, const std::uint32_t lon_partitions)
    : shape3D(topology::TRIANGLE_LIST, model3D::sphere(lat_partitions, lon_partitions, color))
{
    transform.scale = {ra, rb, rc};
}
ellipsoid3D::ellipsoid3D(const float radius, const lynx::color &color, const std::uint32_t lat_partitions,
                         const std::uint32_t lon_partitions)
    : shape3D(topology::TRIANGLE_LIST, model3D::sphere(lat_partitions, lon_partitions, color))
{
    transform.scale = {radius, radius, radius};
}
ellipsoid3D::ellipsoid3D(const lynx::color &color, const std::uint32_t lat_partitions,
                         const std::uint32_t lon_partitions)
    : shape3D(topology::TRIANGLE_LIST, model3D::sphere(lat_partitions, lon_partitions, color))
{
}

float ellipsoid3D::radius() const
{
    return (transform.scale.x + transform.scale.y + transform.scale.z) / 3.f;
}

void ellipsoid3D::radius(const float radius)
{
    transform.scale = {radius, radius, radius};
}

cube3D::cube3D(const glm::vec3 &position, const glm::vec3 &dimensions, const lynx::color &color)
    : shape3D(topology::TRIANGLE_LIST, model3D::cube(color))
{
    transform.position = position;
    transform.scale = dimensions;
}

cube3D::cube3D(const lynx::color &color) : shape3D(topology::TRIANGLE_LIST, model3D::cube(color))
{
}
} // namespace lynx