#include "lynx/internal/pch.hpp"
#include "lynx/drawing/drawable.hpp"

#include "lynx/app/window.hpp"
#include "lynx/drawing/model.hpp"

namespace lynx::drawable
{
void default_draw(window &win, const model *mdl, glm::mat4 transform, topology tplg)
{
    render_system *rs = win.render_system_from_topology(tplg);
    const render_data rdata = rs->create_render_data(mdl, transform);
    rs->push_render_data(rdata);
}
void default_draw_no_transform(window &win, const model *mdl, topology tplg)
{
    default_draw(win, mdl, glm::mat4(1.f), tplg);
}
} // namespace lynx::drawable