#include "lynx/internal/pch.hpp"
#include "lynx/internal/context.hpp"

#include "lynx/drawing/shape.hpp"
#include "lynx/drawing/model.hpp"

#include "lynx/app/window.hpp"

#include "lynx/rendering/buffer.hpp"

#include "lynx/geometry/vertex.hpp"

namespace lynx
{
// Color should already be encoded in arguments when constructing the model
template <class... ModelArgs> shape2D::shape2D(topology tplg, ModelArgs &&...args) : m_topology(tplg)
{
    m_model = make_ref<model2D>(context::current()->device(), std::forward<ModelArgs>(args)...);
}

const glm::vec4 &shape2D::color() const
{
    return m_model->read_vertex(0).color;
}

void shape2D::color(const glm::vec4 &color)
{
    const auto feach = [&color](vertex2D &vtx) { vtx.color = color; };
    m_model->update_vertex_buffer(feach);
}

void shape2D::draw(window2D &win) const
{
    render_system2D &rs = win.render_system(m_topology);
    rs.push_render_data({m_model, transform.transform()});
}

// Color should already be encoded in arguments when constructing the model
template <class... ModelArgs> shape3D::shape3D(ModelArgs &&...args)
{
    m_model = make_ref<model3D>(context::current()->device(), std::forward<ModelArgs>(args)...);
}

const glm::vec4 &shape3D::color() const
{
    return m_model->read_vertex(0).color;
}

void shape3D::color(const glm::vec4 &color)
{
    const auto feach = [&color](vertex3D &vtx) { vtx.color = color; };
    m_model->update_vertex_buffer(feach);
}

rect2D::rect2D(const glm::vec2 &position, const glm::vec2 &dimensions, const glm::vec4 &color)
    : shape2D(TRIANGLE_LIST, model2D::rect(color))
{
    transform.position = position;
    transform.scale = dimensions;
}

rect2D::rect2D(const glm::vec4 &color) : shape2D(TRIANGLE_LIST, model2D::rect(color))
{
}

ellipse2D::ellipse2D(const float ra, const float rb, const glm::vec4 &color, const std::uint32_t partitions)
    : shape2D(TRIANGLE_LIST, model2D::circle(partitions, color))
{
    transform.scale = {ra, rb};
}
ellipse2D::ellipse2D(const float radius, const glm::vec4 &color, const std::uint32_t partitions)
    : shape2D(TRIANGLE_LIST, model2D::circle(partitions, color))
{
    transform.scale = {radius, radius};
}
ellipse2D::ellipse2D(const glm::vec4 &color, const std::uint32_t partitions)
    : shape2D(TRIANGLE_LIST, model2D::circle(partitions, color))
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

polygon2D::polygon2D(const std::vector<glm::vec2> &local_vertices, const glm::vec4 &color)
    : shape2D(TRIANGLE_LIST, model2D::polygon(local_vertices, color)), m_size(local_vertices.size())
{
}

const glm::vec2 &polygon2D::vertex(const std::size_t index) const
{
    return m_model->read_vertex(index + 1).position; // +1 to account for center vertex
}

void polygon2D::vertex(const std::size_t index, const glm::vec2 &vertex)
{
    vertex2D v = m_model->read_vertex(index + 1);
    v.position = vertex;
    m_model->write_vertex(index + 1, v); //+1 to account for center vertex
}

const glm::vec2 &polygon2D::operator[](const std::size_t index) const
{
    return m_model->read_vertex(index + 1).position;
}

std::size_t polygon2D::size() const
{
    return m_size;
}

rect3D::rect3D(const glm::vec3 &position, const glm::vec2 &dimensions, const glm::vec4 &color)
    : shape3D(model3D::rect(color))
{
    transform.position = position;
    transform.scale = glm::vec3(dimensions, 1.f);
}

rect3D::rect3D(const glm::vec4 &color) : shape3D(model3D::rect(color))
{
}

void rect3D::draw(window3D &win) const
{
    render_system3D &rs = win.render_system(TRIANGLE_LIST);
    rs.push_render_data(
        {m_model, transform.transform()}); // TODO: Esto que se implemente en shape3D!! q se pase la topology a su ctor
}

cube3D::cube3D(const glm::vec3 &position, const glm::vec3 &dimensions, const std::array<glm::vec4, 6> &face_colors)
    : shape3D(model3D::cube(face_colors))
{
    transform.position = position;
    transform.scale = dimensions;
}

cube3D::cube3D(const std::array<glm::vec4, 6> &face_colors) : shape3D(model3D::cube(face_colors))
{
}

void cube3D::draw(window3D &win) const
{
    render_system3D &rs = win.render_system(TRIANGLE_LIST);
    rs.push_render_data({m_model, transform.transform()});
}
} // namespace lynx