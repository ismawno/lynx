#include "lynx/internal/pch.hpp"
#include "lynx/drawing/shape.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/buffer.hpp"
#include "kit/utility/utils.hpp"

namespace lynx
{
template <Dimension Dim> const color &shape<Dim>::color() const
{
    return m_model.vertex(0).color;
}
template <Dimension Dim> void shape<Dim>::color(const lynx::color &color)
{
    vertex_t *vdata = m_model.vertex_data();
    for (std::size_t i = 0; i < m_model.vertex_count(); i++)
        vdata[i].color = color;
}

template <Dimension Dim> void shape<Dim>::draw(window_t &win) const
{
    drawable_t::default_draw(win, &m_model, transform.center_scale_rotate_translate4(), m_topology);
}

const color &shape2D::outline_color() const
{
    return m_outline_model.vertex(0).color;
}
void shape2D::outline_color(const lynx::color &color)
{
    vertex2D *vdata = m_outline_model.vertex_data();
    for (std::size_t i = 0; i < m_outline_model.vertex_count(); i++)
        vdata[i].color = color;
}

void shape2D::draw_outline_thickness(window_t &win) const
{
    kit::transform2D<float> outline_transform = transform;
    glm::vec2 mm{FLT_MAX}, mx{-FLT_MAX};

    for (std::size_t i = 0; i < m_outline_model.vertex_count(); i++)
    {
        const glm::vec2 &vertex = m_outline_model.vertex(i).position;
        mm.x = glm::min(mm.x, vertex.x);
        mm.y = glm::min(mm.y, vertex.y);
        mx.x = glm::max(mx.x, vertex.x);
        mx.y = glm::max(mx.y, vertex.y);
    }

    outline_transform.origin = 0.5f * (mx + mm);
    outline_transform.position +=
        kit::transform2D<float>::rotation_matrix(transform.rotation) * outline_transform.origin;
    outline_transform.scale = transform.scale + (2.f * outline_thickness) / (mx - mm);
    drawable::default_draw(win, &m_outline_model, outline_transform.center_scale_rotate_translate4(), m_topology);
}

void shape2D::draw(window_t &win) const
{
    if (!kit::approaches_zero(outline_thickness))
        draw_outline_thickness(win);

    shape::draw(win);
}

template <Dimension Dim>
rect<Dim>::rect(const vec_t &position, const glm::vec2 &dimensions, const lynx::color &color)
    : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::rect(color))
{
    transform.position = position;
    if constexpr (std::is_same_v<Dim, dimension::two>)
        transform.scale = dimensions;
    else
        transform.scale = vec_t(dimensions, 1.f);
}

template <Dimension Dim>
rect<Dim>::rect(const lynx::color &color) : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::rect(color))
{
}

template <Dimension Dim>
ellipse<Dim>::ellipse(const float ra, const float rb, const lynx::color &color, const std::uint32_t partitions)
    : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::circle(partitions, color))
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
        transform.scale = {ra, rb};
    else
        transform.scale = {ra, rb, 1.f};
}
template <Dimension Dim>
ellipse<Dim>::ellipse(const float radius, const lynx::color &color, const std::uint32_t partitions)
    : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::circle(partitions, color))
{
    transform.scale = vec_t(radius);
}
template <Dimension Dim>
ellipse<Dim>::ellipse(const lynx::color &color, const std::uint32_t partitions)
    : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::circle(partitions, color))
{
}

template <Dimension Dim> float ellipse<Dim>::radius() const
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
        return 0.5f * (transform.scale.x + transform.scale.y);
    else
        return (transform.scale.x + transform.scale.y + transform.scale.z) / 3.f;
}

template <Dimension Dim> void ellipse<Dim>::radius(const float radius)
{
    transform.scale = vec_t(radius);
}

template <Dimension Dim>
polygon<Dim>::polygon(const std::vector<vec_t> &local_vertices, const lynx::color &color)
    : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::polygon(local_vertices, color)), m_size(local_vertices.size())
{
}
template <Dimension Dim>
polygon<Dim>::polygon(const std::vector<vertex_t> &local_vertices, const lynx::color &center_color)
    : shape_t(topology::TRIANGLE_LIST, shape_t::model_t::polygon(local_vertices, center_color)),
      m_size(local_vertices.size())
{
}
template <Dimension Dim>
polygon<Dim>::polygon(const lynx::color &color) : polygon({vec_t(0.f), vec_t(0.f), vec_t(0.f)}, color)
{
}

template <Dimension Dim> const vertex<Dim> &polygon<Dim>::operator[](const std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_model.vertex_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertex_count() - 1)
    return m_model.vertex(index + 1); // +1 to account for center vertex
}
template <Dimension Dim> vertex<Dim> &polygon<Dim>::operator[](const std::size_t index)
{
    KIT_ASSERT_ERROR(index < m_model.vertex_count() - 1,
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index,
                     m_model.vertex_count() - 1)
    return m_model.vertex_data()[index + 1]; // +1 to account for center vertex
}

template <Dimension Dim> const lynx::color &polygon<Dim>::center_color() const
{
    return m_model.vertex(0).color;
}

template <Dimension Dim> void polygon<Dim>::center_color(const lynx::color &color)
{
    m_model.vertex_data()[0].color = color;
}

template <Dimension Dim> std::size_t polygon<Dim>::size() const
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

template class shape<dimension::two>;
template class shape<dimension::three>;

template class rect<dimension::two>;
template class rect<dimension::three>;

template class ellipse<dimension::two>;
template class ellipse<dimension::three>;

template class polygon<dimension::two>;
template class polygon<dimension::three>;
} // namespace lynx