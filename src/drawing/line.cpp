#include "lynx/internal/pch.hpp"
#include "lynx/drawing/line.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/buffer.hpp"

namespace lynx
{
template <typename Dim>
thin_line<Dim>::thin_line(const vec_t &p1, const vec_t &p2, const lynx::color &color1, const lynx::color &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform()),
      m_model(context_t::current()->device(), model_t::line(color1, color2))
{
}
template <typename Dim>
thin_line<Dim>::thin_line(const lynx::color &color1, const lynx::color &color2)
    : thin_line(vec_t(-1.f), vec_t(1.f), color1, color2)
{
}
template <typename Dim> thin_line<Dim>::thin_line(const lynx::color &color) : thin_line(color, color)
{
}
template <typename Dim> void thin_line<Dim>::draw(window_t &win) const
{
    drawable_t::default_draw(win, &m_model, m_transform.center_scale_rotate_translate4(), topology::LINE_LIST);
}
template <typename Dim> typename Dim::transform_t thin_line<Dim>::as_transform() const
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
template <typename Dim> const typename Dim::vec_t &thin_line<Dim>::p1() const
{
    return m_p1;
}
template <typename Dim> const typename Dim::vec_t &thin_line<Dim>::p2() const
{
    return m_p2;
}
template <typename Dim> void thin_line<Dim>::p1(const vec_t &p1)
{
    m_p1 = p1;
    m_transform = as_transform();
}
template <typename Dim> void thin_line<Dim>::p2(const vec_t &p2)
{
    m_p2 = p2;
    m_transform = as_transform();
}
template <typename Dim> const color &thin_line<Dim>::color1() const
{
    return m_model.read_vertex(0).color;
}
template <typename Dim> const color &thin_line<Dim>::color2() const
{
    return m_model.read_vertex(1).color;
}
template <typename Dim> const color &thin_line<Dim>::color() const
{
    return m_model.read_vertex(0).color;
}

template <typename T> void update_vertex_color(const std::size_t index, T &model, const color &color)
{
    auto vertex = model[index];
    vertex.color = color;
    model.write_vertex(index, vertex);
}
template <typename Dim> void thin_line<Dim>::color1(const lynx::color &color1)
{
    update_vertex_color(0, m_model, color1);
}
template <typename Dim> void thin_line<Dim>::color2(const lynx::color &color2)
{
    update_vertex_color(1, m_model, color2);
}
template <typename Dim> void thin_line<Dim>::color(const lynx::color &color)
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

template <typename Dim>
line_strip<Dim>::line_strip(const std::vector<vec_t> &points, const lynx::color &color)
    : m_model(context_t::current()->device(), to_vertex_array<vec_t, vertex_t>(points, color))
{
}
template <typename Dim>
line_strip<Dim>::line_strip(const std::vector<vertex_t> &points) : m_model(context_t::current()->device(), points)
{
}

template <typename Dim>
line_strip<Dim>::line_strip(const lynx::color &color) : line_strip({vec_t(-1.f), vec_t(1.f)}, color)
{
}

template <typename Dim> void line_strip<Dim>::draw(window_t &win) const
{
    drawable_t::default_draw_no_transform(win, &m_model, topology::LINE_STRIP);
}

template <typename Dim> const vertex<Dim> &line_strip<Dim>::operator[](const std::size_t index) const
{
    return point(index);
}
template <typename Dim> const vertex<Dim> &line_strip<Dim>::point(const std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count(),
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index, m_model.vertices_count())
    return m_model.read_vertex(index);
}

template <typename Dim> void line_strip<Dim>::point(const std::size_t index, const vertex_t &vertex)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count(),
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index, m_model.vertices_count())
    m_model.write_vertex(index, vertex);
}
template <typename Dim> void line_strip<Dim>::point(const std::size_t index, const vec_t &position)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count(),
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index, m_model.vertices_count())
    vertex_t v = m_model.read_vertex(index);
    v.position = position;
    m_model.write_vertex(index, v);
}

template <typename Dim>
void line_strip<Dim>::update_points(const std::function<void(std::size_t, vertex_t &)> &for_each_fn)
{
    m_model.update_vertex_buffer(for_each_fn);
}

template <typename Dim> const color &line_strip<Dim>::color(const std::size_t index) const
{
    return m_model.read_vertex(index).color;
}

template <typename Dim> void line_strip<Dim>::color(const lynx::color &color)
{
    const auto feach = [&color](std::size_t, vertex_t &vtx) { vtx.color = color; };
    m_model.update_vertex_buffer(feach);
}
template <typename Dim> void line_strip<Dim>::color(std::size_t index, const lynx::color &color)
{
    KIT_ASSERT_ERROR(index < m_model.vertices_count(),
                     "Index exceeds model's vertices count! Index: {0}, vertices: {1}", index, m_model.vertices_count())
    vertex_t v = m_model.read_vertex(index);
    v.color = color;
    m_model.write_vertex(index, v);
}
} // namespace lynx