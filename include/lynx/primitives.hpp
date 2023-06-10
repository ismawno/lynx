#ifndef LYNX_PRIMITIVES_HPP
#define LYNX_PRIMITIVES_HPP

#include "lynx/model.hpp"
#include "lynx/transform.hpp"
#include "lynx/vertex.hpp"

namespace lynx
{
class render_system2D;
class render_system3D;

class drawable2D
{
  public:
    virtual void draw(render_system2D &rs) const = 0;
    virtual topology primitive_topology() const = 0;

    transform2D transform;
};

class drawable3D
{
  public:
    virtual void draw(render_system3D &rs) const = 0;
    virtual topology primitive_topology() const = 0;

    transform3D transform;
};

class line2D : public drawable2D
{
  public:
    line2D(const glm::vec2 &p1, const glm::vec2 &p2);

    void draw(render_system2D &rs) const override;
    topology primitive_topology() const override;

  private:
    using drawable2D::transform;
};

class line3D : public drawable3D
{
  public:
    line3D(const glm::vec3 &p1, const glm::vec3 &p2);

    void draw(render_system3D &rs) const override;
    topology primitive_topology() const override;

  private:
    using drawable3D::transform;
};

class rect2D : public drawable2D
{
  public:
    rect2D(const glm::vec2 &position = {0.f, 0.f}, const glm::vec2 &dimensions = {1.f, 1.f});

    void draw(render_system2D &rs) const override;
    topology primitive_topology() const override;
};

class rect3D : public drawable3D
{
  public:
    rect3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec2 &dimensions = {1.f, 1.f});

    void draw(render_system3D &rs) const override;
    topology primitive_topology() const override;
};

class cube3D : public drawable3D
{
  public:
    cube3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec3 &dimensions = {1.f, 1.f, 1.f});

    void draw(render_system3D &rs) const override;
    topology primitive_topology() const override;
};
} // namespace lynx

#endif