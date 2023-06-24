#include "lynx/pch.hpp"
#include "lynx/line.hpp"
#include "lynx/render_system.hpp"
#include "lynx/window.hpp"
#include "lynx/context.hpp"
#include "lynx/buffer.hpp"

namespace lynx
{
thin_line2D::thin_line2D(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec4 &color1, const glm::vec4 &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform())
{
    m_model = make_ref<model2D>(context::current()->device(), model2D::line(color1, color2));
}

void thin_line2D::draw(window2D &win) const
{
    render_system2D &rs = win.render_system(LINE_LIST);
    rs.push_render_data({m_model, m_transform});
}

transform2D thin_line2D::as_transform() const
{
    transform2D transform;

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

const glm::vec4 &thin_line2D::color1() const
{
    return m_model->read_vertex(0).color;
}
const glm::vec4 &thin_line2D::color2() const
{
    return m_model->read_vertex(1).color;
}

template <typename T> void update_vertex_color(const std::size_t index, T &model, const glm::vec4 &color)
{
    auto vertex = model[index];
    vertex.color = color;
    model.write_vertex(index, vertex);
}

void thin_line2D::color1(const glm::vec4 &color1)
{
    update_vertex_color(0, *m_model, color1);
}
void thin_line2D::color2(const glm::vec4 &color2)
{
    update_vertex_color(1, *m_model, color2);
}

thin_line3D::thin_line3D(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec4 &color1, const glm::vec4 &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform())
{
    m_model = make_ref<model3D>(context::current()->device(), model3D::line(color1, color2));
}

void thin_line3D::draw(window3D &win) const
{
    render_system3D &rs = win.render_system(LINE_LIST);
    rs.push_render_data({m_model, m_transform});
}

transform3D thin_line3D::as_transform() const
{
    transform3D transform;

    const glm::vec3 dp = m_p2 - m_p1;
    transform.position = 0.5f * (m_p1 + m_p2);
    transform.rotation = {0.f, -atan2f(dp.z, dp.x), atan2f(dp.y, dp.x)};
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

const glm::vec4 &thin_line3D::color1() const
{
    return m_model->read_vertex(0).color;
}
const glm::vec4 &thin_line3D::color2() const
{
    return m_model->read_vertex(1).color;
}

void thin_line3D::color1(const glm::vec4 &color1)
{
    update_vertex_color(0, *m_model, color1);
}
void thin_line3D::color2(const glm::vec4 &color2)
{
    update_vertex_color(1, *m_model, color2);
}
} // namespace lynx