#include "lynx/internal/pch.hpp"
#include "lynx/drawing/line.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/buffer.hpp"

namespace lynx
{
template <Dimension Dim>
thin_line<Dim>::thin_line(const vec_t &p1, const vec_t &p2, const lynx::color &color1, const lynx::color &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform()), m_model(context_t::device(), model_t::line(color1, color2))
{
}
template <Dimension Dim>
thin_line<Dim>::thin_line(const lynx::color &color1, const lynx::color &color2)
    : thin_line(vec_t(-1.f), vec_t(1.f), color1, color2)
{
}
template <Dimension Dim> thin_line<Dim>::thin_line(const lynx::color &color) : thin_line(color, color)
{
}
template <Dimension Dim> void thin_line<Dim>::draw(window_t &win) const
{
    drawable_t::default_draw(win, &m_model, m_transform.center_scale_rotate_translate4(), topology::LINE_LIST);
}
template <Dimension Dim> typename Dim::transform_t thin_line<Dim>::as_transform() const
{
    transform_t transform{};

    const vec_t dp = m_p2 - m_p1;
    transform.position = 0.5f * (m_p1 + m_p2);
    if constexpr (std::is_same_v<Dim, dimension::two>)
        transform.rotation = atan2f(dp.y, dp.x);
    else
        transform.rotation = transform_t::YZ(-atan2f(dp.z, dp.x), atan2f(dp.y, dp.x));
    transform.scale.x = 0.5f * glm::length(dp);
    return transform;
}
template <Dimension Dim> const glm::vec<Dim::N, float> &thin_line<Dim>::p1() const
{
    return m_p1;
}
template <Dimension Dim> const glm::vec<Dim::N, float> &thin_line<Dim>::p2() const
{
    return m_p2;
}
template <Dimension Dim> void thin_line<Dim>::p1(const vec_t &p1)
{
    m_p1 = p1;
    m_transform = as_transform();
}
template <Dimension Dim> void thin_line<Dim>::p2(const vec_t &p2)
{
    m_p2 = p2;
    m_transform = as_transform();
}
template <Dimension Dim> const color &thin_line<Dim>::color1() const
{
    return m_model.vertex(0).color;
}
template <Dimension Dim> const color &thin_line<Dim>::color2() const
{
    return m_model.vertex(1).color;
}
template <Dimension Dim> const color &thin_line<Dim>::color() const
{
    return m_model.vertex(0).color;
}
template <Dimension Dim> const typename Dim::transform_t *thin_line<Dim>::parent() const
{
    return m_transform.parent;
}
template <Dimension Dim> void thin_line<Dim>::parent(const transform_t *parent)
{
    m_transform.parent = parent;
}

template <Dimension Dim> void thin_line<Dim>::color1(const lynx::color &color1)
{
    m_model.vertex_data()[0].color = color1;
}
template <Dimension Dim> void thin_line<Dim>::color2(const lynx::color &color2)
{
    m_model.vertex_data()[1].color = color2;
}
template <Dimension Dim> void thin_line<Dim>::color(const lynx::color &color)
{
    color1(color);
    color2(color);
}

template <typename T1, typename T2>
static std::vector<T2> to_vertex_array(const std::vector<T1> &points, const color &color)
{
    std::vector<T2> result;
    result.reserve(points.size());
    for (const T1 &point : points)
        result.emplace_back(point, color);
    return result;
}

template <Dimension Dim>
line_strip<Dim>::line_strip(const std::vector<vec_t> &points, const lynx::color &color)
    : m_model(context_t::device(), to_vertex_array<vec_t, vertex_t>(points, color))
{
}
template <Dimension Dim>
line_strip<Dim>::line_strip(const std::vector<vertex_t> &points) : m_model(context_t::device(), points)
{
}

template <Dimension Dim>
line_strip<Dim>::line_strip(const lynx::color &color) : line_strip({vec_t(-1.f), vec_t(1.f)}, color)
{
}

template <Dimension Dim> void line_strip<Dim>::draw(window_t &win) const
{
    drawable_t::default_draw(win, &m_model, m_transform.center_scale_rotate_translate4(), topology::LINE_STRIP);
}

template <Dimension Dim> const typename Dim::transform_t *line_strip<Dim>::parent() const
{
    return m_transform.parent;
}
template <Dimension Dim> void line_strip<Dim>::parent(const transform_t *parent)
{
    m_transform.parent = parent;
}

template <Dimension Dim> const vertex<Dim> &line_strip<Dim>::operator[](const std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_model.vertex_count(), "Index exceeds model's vertices count! Index: {0}, vertices: {1}",
                     index, m_model.vertex_count())
    return m_model.vertex(index);
}

template <Dimension Dim> vertex<Dim> &line_strip<Dim>::operator[](const std::size_t index)
{
    KIT_ASSERT_ERROR(index < m_model.vertex_count(), "Index exceeds model's vertices count! Index: {0}, vertices: {1}",
                     index, m_model.vertex_count())
    return m_model.vertex_data()[index];
}

template <Dimension Dim> const color &line_strip<Dim>::color() const
{
    return m_model.vertex(0).color;
}

template <Dimension Dim> void line_strip<Dim>::color(const lynx::color &color)
{
    vertex_t *vdata = m_model.vertex_data();
    for (std::size_t i = 0; i < m_model.vertex_count(); i++)
        vdata[i].color = color;
}

template class thin_line<dimension::two>;
template class thin_line<dimension::three>;

template class line_strip<dimension::two>;
template class line_strip<dimension::three>;
} // namespace lynx