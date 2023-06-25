#ifndef LYNX_SHAPE_HPP
#define LYNX_SHAPE_HPP

#include "lynx/drawable.hpp"
#include "lynx/core.hpp"
#include "lynx/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
class model2D;
class model3D;

struct vertex2D;
struct vertex3D;

class shape2D : public drawable2D
{
  public:
    template <class... ModelArgs> shape2D(ModelArgs &&...args);

    const glm::vec4 &color() const;
    void color(const glm::vec4 &color);

    transform2D transform;

  protected:
    ref<model2D> m_model;
};

class shape3D : public drawable3D
{
  public:
    template <class... ModelArgs> shape3D(ModelArgs &&...args);

    const glm::vec4 &color() const;
    void color(const glm::vec4 &color);

    transform3D transform;

  protected:
    ref<model3D> m_model;
};

class rect2D : public shape2D
{
  public:
    rect2D(const glm::vec2 &position = {0.f, 0.f}, const glm::vec2 &dimensions = {1.f, 1.f},
           const glm::vec4 &color = glm::vec4(1.f));
    rect2D(const glm::vec4 &color);

    void draw(window2D &win) const override;

    static inline constexpr topology TOPOLOGY = TRIANGLE_LIST;
};
class ellipse2D : public shape2D
{
  public:
    ellipse2D(float ra, float rb, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t partitions = 30);
    ellipse2D(float radius = 1.f, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t partitions = 30);
    ellipse2D(const glm::vec4 &color, std::uint32_t partitions = 30);

    float radius() const;
    void radius(float radius);

    void draw(window2D &win) const override;
};
class polygon2D : public shape2D
{
  public:
    polygon2D(const std::vector<glm::vec3> &vertices, const glm::vec4 &color);
    polygon2D(const std::vector<vertex2D> &vertices);

    void draw(window2D &win) const override;
};

class rect3D : public shape3D
{
  public:
    rect3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec2 &dimensions = {1.f, 1.f},
           const glm::vec4 &color = glm::vec4(1.f));
    rect3D(const glm::vec4 &color);

    void draw(window3D &win) const override;
};
class circle3D : public shape3D
{
};
class polygon3D : public shape3D
{
};
class cube3D : public shape3D
{
  public:
    cube3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec3 &dimensions = {1.f, 1.f, 1.f},
           const std::array<glm::vec4, 6> &face_colors = {
               glm::vec4(.9f, .9f, .9f, 1.f), glm::vec4(.8f, .8f, .1f, 1.f), glm::vec4(.9f, .6f, .1f, 1.f),
               glm::vec4(.8f, .1f, .1f, 1.f), glm::vec4(.1f, .1f, .8f, 1.f), glm::vec4(.1f, .8f, .1f, 1.f)});
    cube3D(const std::array<glm::vec4, 6> &face_colors);

    void draw(window3D &win) const override;
};
} // namespace lynx

#endif