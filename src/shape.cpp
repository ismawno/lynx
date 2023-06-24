#include "lynx/pch.hpp"
#include "lynx/shape.hpp"
#include "lynx/context.hpp"
#include "lynx/vertex.hpp"
#include "lynx/model.hpp"
#include "lynx/window.hpp"
#include "lynx/buffer.hpp"

namespace lynx
{
// Color should already be encoded in arguments when constructing the model
template <class... ModelArgs> shape2D::shape2D(const glm::vec4 &color, ModelArgs &&...args) : m_color(color)
{
    m_model = make_ref<model2D>(std::forward<ModelArgs>(args)...);
}

const glm::vec4 &shape2D::color() const
{
    return m_color;
}

void shape2D::color(const glm::vec4 &color)
{
    const auto feach = [&color](vertex2D &vtx) { vtx.color = color; };
    m_model->update_vertex_buffer(feach);
}

// Color should already be encoded in arguments when constructing the model
template <class... ModelArgs> shape3D::shape3D(const glm::vec4 &color, ModelArgs &&...args) : m_color(color)
{
    m_model = make_ref<model3D>(std::forward<ModelArgs>(args)...);
}

const glm::vec4 &shape3D::color() const
{
    return m_color;
}

void shape3D::color(const glm::vec4 &color)
{
    const auto feach = [&color](vertex3D &vtx) { vtx.color = color; };
    m_model->update_vertex_buffer(feach);
}

rect2D::rect2D(const glm::vec2 &position, const glm::vec2 &dimensions, const glm::vec4 &color)
    : shape2D(color, context::current()->device(), model2D::rect(color))
{
    transform.position = position;
    transform.scale = dimensions;
}

rect2D::rect2D(const glm::vec4 &color) : shape2D(color, context::current()->device(), model2D::rect(color))
{
}

void rect2D::draw(window2D &win) const
{
    render_system2D &rs = win.render_system(TRIANGLE_LIST);
    rs.push_render_data({m_model, transform});
}

rect3D::rect3D(const glm::vec3 &position, const glm::vec2 &dimensions, const glm::vec4 &color)
    : shape3D(color, context::current()->device(), model3D::rect(color))
{
    transform.position = position;
    transform.scale = glm::vec3(dimensions, 1.f);
}

rect3D::rect3D(const glm::vec4 &color) : shape3D(color, context::current()->device(), model3D::rect(color))
{
}

void rect3D::draw(window3D &win) const
{
    render_system3D &rs = win.render_system(TRIANGLE_LIST);
    rs.push_render_data({m_model, transform});
}

cube3D::cube3D(const glm::vec3 &position, const glm::vec3 &dimensions, const std::array<glm::vec4, 6> &face_colors)
    : shape3D(face_colors[0], context::current()->device(), model3D::cube(face_colors))
{
    transform.position = position;
    transform.scale = dimensions;
}

cube3D::cube3D(const std::array<glm::vec4, 6> &face_colors)
    : shape3D(face_colors[0], context::current()->device(), model3D::cube(face_colors))
{
}

void cube3D::draw(window3D &win) const
{
    render_system3D &rs = win.render_system(TRIANGLE_LIST);
    rs.push_render_data({m_model, transform});
}
} // namespace lynx