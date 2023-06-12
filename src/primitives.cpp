#include "lynx/pch.hpp"
#include "lynx/primitives.hpp"
#include "lynx/render_systems.hpp"

namespace lynx
{
line2D::line2D(const glm::vec2 &p1, const glm::vec2 &p2)
{
    const glm::vec2 dp = p2 - p1;

    transform.position = 0.5f * (p1 + p2);
    transform.rotation = atan2f(dp.y, dp.x);
    transform.scale.x = 0.5f * glm::length(dp);
}

void line2D::draw(render_system2D &rs) const
{
    static ref<model2D> mdl = rs.model_from_vertices(model2D::line());
    rs.push_render_data({mdl, {transform}});
}

topology line2D::primitive_topology() const
{
    return LINE_LIST;
}

line3D::line3D(const glm::vec3 &p1, const glm::vec3 &p2)
{
    const glm::vec3 dp = p2 - p1;

    transform.position = 0.5f * (p1 + p2);
    transform.rotation = {0.f, -atan2f(dp.z, dp.x), atan2f(dp.y, dp.x)};
    // DBG_INFO("{0}", atan2f(dp.x, dp.z))
    transform.scale.x = 0.5f * glm::length(dp);
}

void line3D::draw(render_system3D &rs) const
{
    static ref<model3D> mdl = rs.model_from_vertices(model3D::line());
    rs.push_render_data({mdl, {transform}});
}

topology line3D::primitive_topology() const
{
    return LINE_LIST;
}

rect2D::rect2D(const glm::vec2 &position, const glm::vec2 &dimensions)
{
    transform.position = position;
    transform.scale = dimensions;
}

void rect2D::draw(render_system2D &rs) const
{
    static ref<model2D> mdl = rs.model_from_vertices(model2D::rect());
    rs.push_render_data({mdl, {transform}});
}

topology rect2D::primitive_topology() const
{
    return TRIANGLE_LIST;
}

rect3D::rect3D(const glm::vec3 &position, const glm::vec2 &dimensions)
{
    transform.position = position;
    transform.scale = {dimensions.x, dimensions.y, 1.f};
}

void rect3D::draw(render_system3D &rs) const
{
    static ref<model3D> mdl = rs.model_from_vertices(model3D::rect());
    rs.push_render_data({mdl, {transform}});
}

topology rect3D::primitive_topology() const
{
    return TRIANGLE_LIST;
}

cube3D::cube3D(const glm::vec3 &position, const glm::vec3 &dimensions)
{
    transform.position = position;
    transform.scale = dimensions;
}

void cube3D::draw(render_system3D &rs) const
{
    static ref<model3D> mdl = rs.model_from_vertices(model3D::cube());
    rs.push_render_data({mdl, {transform}});
}

topology cube3D::primitive_topology() const
{
    return TRIANGLE_LIST;
}
} // namespace lynx