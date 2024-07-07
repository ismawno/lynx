#include "lynx/internal/pch.hpp"
#include "lynx/drawing/drawable.hpp"

#include "lynx/app/window.hpp"
#include "lynx/drawing/model.hpp"

namespace lynx
{
template <Dimension Dim> modelable<Dim>::modelable(const modelable &other)
{
    if (other.m_model)
        m_model = kit::make_ref<model_t>(*other.m_model);
}
template <Dimension Dim> modelable<Dim> &modelable<Dim>::operator=(const modelable &other)
{
    if (this != &other)
        m_model = kit::make_ref<model_t>(*other.m_model);
    return *this;
}

template <Dimension Dim>
void drawable<Dim>::default_draw(window_t &win, const kit::ref<const model_t> &mdl, glm::mat4 transform, topology tplg)
{
    render_system_t *rs = win.render_system_from_topology(tplg);
    const typename render_system_t::render_data rdata = rs->create_render_data(mdl, transform);
    rs->push_render_data(rdata);
}
template <Dimension Dim>
void drawable<Dim>::default_draw_no_transform(window_t &win, const kit::ref<const model_t> &mdl, topology tplg)
{
    default_draw(win, mdl, glm::mat4(1.f), tplg);
}

template class drawable<dimension::two>;
template class drawable<dimension::three>;

template class modelable<dimension::two>;
template class modelable<dimension::three>;
} // namespace lynx