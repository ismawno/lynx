#pragma once

#include "lynx/drawing/drawable.hpp"
#include "kit/utility/transform.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/geometry/vertex.hpp"
#include "lynx/internal/context.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define GLM_FORCE_AVX2
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace lynx
{
template <Dimension Dim> class shape : public drawable<Dim>, public modelable<Dim>
{
  public:
    using transform_t = typename Dim::transform_t;
    using model_t = typename Dim::model_t;
    using window_t = window<Dim>;
    using context_t = context<Dim>;
    using drawable_t = drawable<Dim>;
    using vertex_t = vertex<Dim>;

    template <class... ModelArgs>
    shape(topology tplg, ModelArgs &&...args)
        : modelable<Dim>(context_t::device(), std::forward<ModelArgs>(args)...), m_topology(tplg)
    {
    }

    const lynx::color &color() const;
    void color(const lynx::color &color);

    transform_t transform{};

  protected:
    topology m_topology;
    virtual void draw(window_t &win) const override;
};

class shape2D : public shape<dimension::two>
{
  public:
    template <class... ModelArgs>
    shape2D(topology tplg, ModelArgs &&...args)
        : shape(tplg, std::forward<ModelArgs>(args)...),
          m_outline_model(kit::make_ref<model_t>(context_t::device(), std::forward<ModelArgs>(args)...))
    {
    }

    shape2D(const shape2D &other);
    shape2D &operator=(const shape2D &other);

    shape2D(shape2D &&other) = default;
    shape2D &operator=(shape2D &&other) = default;

    const lynx::color &outline_color() const;
    void outline_color(const lynx::color &color);

    float outline_thickness = 0.f;

  protected:
    virtual void draw(window_t &win) const override;

  private:
    kit::ref<model_t> m_outline_model;

    void draw_outline_thickness(window_t &win) const;
};

using shape3D = shape<dimension::three>;

template <Dimension Dim> class rect : public Dim::shape_t
{
  public:
    using vec_t = glm::vec<Dim::N, float>;
    using shape_t = typename Dim::shape_t;
    using shape_t::transform;

    rect(const vec_t &position = vec_t(0.f), const glm::vec2 &dimensions = {1.f, 1.f},
         const lynx::color &color = lynx::color::white);
    rect(const lynx::color &color);
};

using rect2D = rect<dimension::two>;
using rect3D = rect<dimension::three>;

template <Dimension Dim> class ellipse : public Dim::shape_t
{
  public:
    using vec_t = glm::vec<Dim::N, float>;
    using shape_t = typename Dim::shape_t;
    using shape_t::transform;

    ellipse(float ra, float rb, const lynx::color &color = lynx::color::white, std::uint32_t partitions = 30);
    ellipse(float radius = 1.f, const lynx::color &color = lynx::color::white, std::uint32_t partitions = 30);
    ellipse(const lynx::color &color, std::uint32_t partitions = 30);

    float radius() const;
    void radius(float radius);
};

using ellipse2D = ellipse<dimension::two>;
using ellipse3D = ellipse<dimension::three>;

template <Dimension Dim> class polygon : public Dim::shape_t
{
  public:
    using vec_t = glm::vec<Dim::N, float>;
    using vertex_t = vertex<Dim>;
    using shape_t = typename Dim::shape_t;

    using shape_t::transform;

    polygon(const std::vector<vec_t> &local_vertices = {vec_t(0.f), vec_t(0.f), vec_t(0.f)},
            const lynx::color &color = lynx::color::white);
    polygon(const std::vector<vertex_t> &local_vertices, const lynx::color &center_color = lynx::color::white);
    polygon(const lynx::color &color);

    const vertex_t &operator[](std::size_t index) const;
    vertex_t &operator[](std::size_t index);

    std::size_t size() const;

    const lynx::color &center_color() const;
    void center_color(const lynx::color &color);

  private:
    using shape_t::m_model;
    std::size_t m_size;
};

using polygon2D = polygon<dimension::two>;
using polygon3D = polygon<dimension::three>;

class ellipsoid3D : public shape3D
{
  public:
    ellipsoid3D(float ra, float rb, float rc, const lynx::color &color = lynx::color::white,
                std::uint32_t lat_partitions = 30, std::uint32_t lon_partitions = 15);
    ellipsoid3D(float radius = 1.f, const lynx::color &color = lynx::color::white, std::uint32_t lat_partitions = 30,
                std::uint32_t lon_partitions = 15);
    ellipsoid3D(const lynx::color &color, std::uint32_t lat_partitions = 30, std::uint32_t lon_partitions = 15);

    float radius() const;
    void radius(float radius);
};
class cube3D : public shape3D
{
  public:
    cube3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec3 &dimensions = glm::vec3(1.f),
           const lynx::color &color = lynx::color::white);
    cube3D(const lynx::color &color);
};
} // namespace lynx
