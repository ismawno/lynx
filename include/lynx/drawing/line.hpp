#pragma once

#include "lynx/drawing/drawable.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/geometry/vertex.hpp"
#include "kit/utility/transform.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/internal/dimension.hpp"
#include "lynx/internal/context.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace lynx
{
template <Dimension Dim> class line : public drawable<Dim>
{
  public:
    using vec_t = glm::vec<Dim::N, float>;
    virtual const vec_t &p1() const = 0;
    virtual const vec_t &p2() const = 0;

    virtual void p1(const vec_t &p1) = 0;
    virtual void p2(const vec_t &p2) = 0;

    virtual const color &color() const = 0;
    virtual void color(const lynx::color &color) = 0;
};

using line2D = line<dimension::two>;
using line3D = line<dimension::three>;

template <Dimension Dim> class thin_line : public line<Dim>, public modelable<Dim>
{
  public:
    using vec_t = glm::vec<Dim::N, float>;
    using window_t = window<Dim>;
    using transform_t = typename Dim::transform_t;
    using model_t = typename Dim::model_t;
    using context_t = context<Dim>;
    using drawable_t = drawable<Dim>;

    thin_line(const vec_t &p1 = vec_t(-1.f), const vec_t &p2 = vec_t(1.f),
              const lynx::color &color = lynx::color::white);
    thin_line(const vec_t &p1, const vec_t &p2, const lynx::color &color1, const lynx::color &color2);
    thin_line(const lynx::color &color1, const lynx::color &color2);
    thin_line(const lynx::color &color);

    void draw(window_t &win) const override final;

    const vec_t &p1() const override final;
    const vec_t &p2() const override final;

    void p1(const vec_t &p1) override final;
    void p2(const vec_t &p2) override final;

    const lynx::color &color1() const;
    const lynx::color &color2() const;
    const lynx::color &color() const override final;

    void color1(const lynx::color &color1);
    void color2(const lynx::color &color2);
    void color(const lynx::color &color) override final;

    const transform_t *parent() const;
    void parent(const transform_t *parent);

  private:
    vec_t m_p1;
    vec_t m_p2;

    transform_t m_transform;
    transform_t as_transform() const;
};

using thin_line2D = thin_line<dimension::two>;
using thin_line3D = thin_line<dimension::three>;

template <Dimension Dim> class line_strip : public drawable<Dim>, public modelable<Dim>
{
  public:
    using vec_t = glm::vec<Dim::N, float>;
    using window_t = window<Dim>;
    using model_t = typename Dim::model_t;
    using transform_t = typename Dim::transform_t;
    using vertex_t = vertex<Dim>;
    using context_t = context<Dim>;
    using drawable_t = drawable<Dim>;

    line_strip(const std::vector<vec_t> &points = {{0.f, 0.f}, {1.f, 0.f}}, const color &color = color::white);
    line_strip(const std::vector<vertex_t> &points);
    line_strip(const color &color);

    void draw(window_t &win) const override;

    const vertex_t &operator[](std::size_t index) const;
    vertex_t &operator[](std::size_t index);

    const color &color() const;
    void color(const lynx::color &color);

    const transform_t *parent() const;
    void parent(const transform_t *parent);

  private:
    transform_t m_transform;
};

using line_strip2D = line_strip<dimension::two>;
using line_strip3D = line_strip<dimension::three>;

} // namespace lynx
