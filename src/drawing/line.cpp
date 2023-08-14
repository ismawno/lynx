#include "lynx/internal/pch.hpp"
#include "lynx/drawing/line.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/app/window.hpp"
#include "lynx/utility/context.hpp"
#include "lynx/rendering/buffer.hpp"

namespace lynx
{
thin_line2D::thin_line2D(const glm::vec2 &p1, const glm::vec2 &p2, const lynx::color &color1, const lynx::color &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform()),
      m_model(context::current()->device(), model2D::line(color1, color2))
{
}

void thin_line2D::draw(window2D &win) const
{
    drawable::default_draw(win, &m_model, m_transform.center_scale_rotate_translate4(), topology::LINE_LIST);
}

kit::transform2D thin_line2D::as_transform() const
{
    kit::transform2D transform{};

    const glm::vec2 dp = m_p2 - m_p1;
    transform.position = 0.5f * (m_p1 + m_p2);
    transform.rotation = atan2f(dp.y, dp.x);
    transform.scale.x = 0.5f * glm::length(dp);
    return transform;
}

const glm::vec2 &thin_line2D::p1() const
{
    return m_p1;
}
const glm::vec2 &thin_line2D::p2() const
{
    return m_p2;
}

void thin_line2D::p1(const glm::vec2 &p1)
{
    m_p1 = p1;
    m_transform = as_transform();
}
void thin_line2D::p2(const glm::vec2 &p2)
{
    m_p2 = p2;
    m_transform = as_transform();
}

const color &thin_line2D::color1() const
{
    return m_model.read_vertex(0).color;
}
const color &thin_line2D::color2() const
{
    return m_model.read_vertex(1).color;
}
const color &thin_line2D::color() const
{
    return m_model.read_vertex(0).color;
}

template <typename T> void update_vertex_color(const std::size_t index, T &model, const color &color)
{
    auto vertex = model[index];
    vertex.color = color;
    model.write_vertex(index, vertex);
}

void thin_line2D::color1(const lynx::color &color1)
{
    update_vertex_color(0, m_model, color1);
}
void thin_line2D::color2(const lynx::color &color2)
{
    update_vertex_color(1, m_model, color2);
}
void thin_line2D::color(const lynx::color &color)
{
    color1(color);
    color2(color);
}

thin_line3D::thin_line3D(const glm::vec3 &p1, const glm::vec3 &p2, const lynx::color &color1, const lynx::color &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform()),
      m_model(context::current()->device(), model3D::line(color1, color2))
{
}

void thin_line3D::draw(window3D &win) const
{
    drawable::default_draw(win, &m_model, m_transform.center_scale_rotate_translate4(), topology::LINE_LIST);
}

kit::transform3D thin_line3D::as_transform() const
{
    kit::transform3D transform{};

    const glm::vec3 dp = m_p2 - m_p1;
    transform.position = 0.5f * (m_p1 + m_p2);
    transform.rotation = kit::transform3D::YZ(-atan2f(dp.z, dp.x), atan2f(dp.y, dp.x));
    transform.scale.x = 0.5f * glm::length(dp);
    return transform;
}

const glm::vec3 &thin_line3D::p1() const
{
    return m_p1;
}
const glm::vec3 &thin_line3D::p2() const
{
    return m_p2;
}

void thin_line3D::p1(const glm::vec3 &p1)
{
    m_p1 = p1;
    m_transform = as_transform();
}
void thin_line3D::p2(const glm::vec3 &p2)
{
    m_p2 = p2;
    m_transform = as_transform();
}

const color &thin_line3D::color1() const
{
    return m_model.read_vertex(0).color;
}
const color &thin_line3D::color2() const
{
    return m_model.read_vertex(1).color;
}
const color &thin_line3D::color() const
{
    return m_model.read_vertex(0).color;
}

void thin_line3D::color1(const lynx::color &color1)
{
    update_vertex_color(0, m_model, color1);
}
void thin_line3D::color2(const lynx::color &color2)
{
    update_vertex_color(1, m_model, color2);
}
void thin_line3D::color(const lynx::color &color)
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

line_strip2D::line_strip2D(const std::vector<glm::vec2> &points, const lynx::color &color)
    : m_model(context::current()->device(), to_vertex_array<glm::vec2, vertex2D>(points, color))
{
}

line_strip2D::line_strip2D(const std::vector<vertex2D> &points) : m_model(context::current()->device(), points)
{
}

void line_strip2D::draw(window2D &win) const
{
    drawable::default_draw_no_transform(win, &m_model, topology::LINE_STRIP);
}

const vertex2D &line_strip2D::operator[](const std::size_t index) const
{
    return m_model.read_vertex(index);
}
const vertex2D &line_strip2D::point(const std::size_t index) const
{
    return m_model.read_vertex(index);
}
void line_strip2D::point(const std::size_t index, const vertex2D &vertex)
{
    m_model.write_vertex(index, vertex);
}

void line_strip2D::update_points(const std::function<void(vertex2D &)> &for_each_fn)
{
    m_model.update_vertex_buffer(for_each_fn);
}

const color &line_strip2D::color() const
{
    return m_model.read_vertex(0).color;
}

void line_strip2D::color(const lynx::color &color)
{
    const auto feach = [&color](vertex2D &vtx) { vtx.color = color; };
    m_model.update_vertex_buffer(feach);
}

line_strip3D::line_strip3D(const std::vector<glm::vec3> &points, const lynx::color &color)
    : m_model(context::current()->device(), to_vertex_array<glm::vec3, vertex3D>(points, color))
{
}

line_strip3D::line_strip3D(const std::vector<vertex3D> &points) : m_model(context::current()->device(), points)
{
}

void line_strip3D::draw(window3D &win) const
{
    drawable::default_draw_no_transform(win, &m_model, topology::LINE_STRIP);
}

const vertex3D &line_strip3D::operator[](const std::size_t index) const
{
    return m_model.read_vertex(index);
}
const vertex3D &line_strip3D::point(std::size_t index) const
{
    return m_model.read_vertex(index);
}
void line_strip3D::point(std::size_t index, const vertex3D &vertex)
{
    m_model.write_vertex(index, vertex);
}

void line_strip3D::update_points(const std::function<void(vertex3D &)> &for_each_fn)
{
    m_model.update_vertex_buffer(for_each_fn);
}

const color &line_strip3D::color() const
{
    return m_model.read_vertex(0).color;
}

void line_strip3D::color(const lynx::color &color)
{
    const auto feach = [&color](vertex3D &vtx) { vtx.color = color; };
    m_model.update_vertex_buffer(feach);
}
} // namespace lynx