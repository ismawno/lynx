#include "lynx/pch.hpp"
#include "lynx/primitives.hpp"
#include "lynx/render_systems.hpp"

namespace lynx
{
line2D::line2D(const glm::vec2 &p1, const glm::vec2 &p2)
{
    const glm::vec2 dp = p2 - p1;

    transform.translation = 0.5f * (p1 + p2);
    transform.rotation = atan2f(dp.y, dp.x);
    transform.scale = 0.5f * dp;
}

void line2D::draw(render_system2D &rs) const
{
    static ref<model2D> mdl = rs.model_from_vertices(model2D::line());
    rs.push_render_data({mdl, {transform}});
}
topology line2D::primitive_topology() const
{
    return LINE;
}
} // namespace lynx