#include "lynx/internal/pch.hpp"
#include "lynx/drawing/line.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/app/window.hpp"
#include "lynx/internal/context.hpp"
#include "lynx/rendering/buffer.hpp"

namespace lynx
{
line2D::line2D(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec4 &color1, const glm::vec4 &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform()),
      m_model(context::current()->device(), model2D::line(color1, color2))
{
}

void line2D::draw(window2D &win) const
{
    render_system2D &rs = win.render_system(LINE_LIST);
    glm::mat4 tmat = m_transform.transform();
    const render_data rdata = rs.create_render_data(&m_model, tmat);
    rs.push_render_data(rdata);
}

transform2D line2D::as_transform() const
{
    transform2D transform{};

    const glm::vec2 dp = m_p2 - m_p1;
    transform.position = 0.5f * (m_p1 + m_p2);
    transform.rotation = atan2f(dp.y, dp.x);
    transform.scale.x = 0.5f * glm::length(dp);
    return transform;
}

const glm::vec2 &line2D::p1() const
{
    return m_p1;
}
const glm::vec2 &line2D::p2() const
{
    return m_p2;
}

void line2D::p1(const glm::vec2 &p1)
{
    m_p1 = p1;
    m_transform = as_transform();
}
void line2D::p2(const glm::vec2 &p2)
{
    m_p2 = p2;
    m_transform = as_transform();
}

const glm::vec4 &line2D::color1() const
{
    return m_model.read_vertex(0).color;
}
const glm::vec4 &line2D::color2() const
{
    return m_model.read_vertex(1).color;
}

template <typename T> void update_vertex_color(const std::size_t index, T &model, const glm::vec4 &color)
{
    auto vertex = model[index];
    vertex.color = color;
    model.write_vertex(index, vertex);
}

void line2D::color1(const glm::vec4 &color1)
{
    update_vertex_color(0, m_model, color1);
}
void line2D::color2(const glm::vec4 &color2)
{
    update_vertex_color(1, m_model, color2);
}

line3D::line3D(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec4 &color1, const glm::vec4 &color2)
    : m_p1(p1), m_p2(p2), m_transform(as_transform()),
      m_model(context::current()->device(), model3D::line(color1, color2))
{
}

void line3D::draw(window3D &win) const
{
    render_system3D &rs = win.render_system(LINE_LIST);
    glm::mat4 tmat = m_transform.transform();
    const render_data rdata = rs.create_render_data(&m_model, tmat);
    rs.push_render_data(rdata);
}

transform3D line3D::as_transform() const
{
    transform3D transform{};

    const glm::vec3 dp = m_p2 - m_p1;
    transform.position = 0.5f * (m_p1 + m_p2);
    transform.rotation = transform3D::YZ(-atan2f(dp.z, dp.x), atan2f(dp.y, dp.x));
    transform.scale.x = 0.5f * glm::length(dp);
    return transform;
}

const glm::vec3 &line3D::p1() const
{
    return m_p1;
}
const glm::vec3 &line3D::p2() const
{
    return m_p2;
}

void line3D::p1(const glm::vec3 &p1)
{
    m_p1 = p1;
    m_transform = as_transform();
}
void line3D::p2(const glm::vec3 &p2)
{
    m_p2 = p2;
    m_transform = as_transform();
}

const glm::vec4 &line3D::color1() const
{
    return m_model.read_vertex(0).color;
}
const glm::vec4 &line3D::color2() const
{
    return m_model.read_vertex(1).color;
}

void line3D::color1(const glm::vec4 &color1)
{
    update_vertex_color(0, m_model, color1);
}
void line3D::color2(const glm::vec4 &color2)
{
    update_vertex_color(1, m_model, color2);
}
} // namespace lynx