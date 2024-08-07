#pragma once

#include "lynx/internal/dimension.hpp"
#include "kit/memory/ptr/ref.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>

namespace lynx
{

template <Dimension Dim> class modelable
{
  protected:
    using model_t = typename Dim::model_t;

    template <class... ModelArgs>
    modelable(ModelArgs &&...args) : m_model(kit::make_ref<model_t>(std::forward<ModelArgs>(args)...))
    {
    }

    modelable(const modelable &other);
    modelable &operator=(const modelable &other);

    modelable(modelable &&other) = default;
    modelable &operator=(modelable &&other) = default;

    kit::ref<model_t> m_model;
};

enum class topology
{
    POINT_LIST = 0,
    LINE_LIST = 1,
    LINE_STRIP = 2,
    TRIANGLE_LIST = 3,
    TRIANGLE_STRIP = 4
};

template <Dimension Dim> class drawable
{
  public:
    using window_t = window<Dim>;
    using model_t = typename Dim::model_t;
    using render_system_t = render_system<Dim>;

    virtual ~drawable() = default;

    virtual void draw(window_t &win) const = 0;
    virtual void draw(render_system_t &rs) const
    {
        KIT_ERROR("To draw to an arbitrary render system, the draw render system method must be overriden")
    }

    static void default_draw(window_t &win, const kit::ref<const model_t> &mdl, glm::mat4 transform, topology tplg);
    static void default_draw_no_transform(window_t &win, const kit::ref<const model_t> &mdl, topology tplg);
};

using drawable2D = drawable<dimension::two>;
using drawable3D = drawable<dimension::three>;
} // namespace lynx
