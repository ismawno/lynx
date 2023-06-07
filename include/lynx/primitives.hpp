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
    transform2D transform;
    virtual void draw(render_system2D &rs) const = 0;
    virtual topology primitive_topology() const = 0;
};

class drawable3D
{
  public:
    transform3D transform;
    virtual void draw(const render_system3D &rs) const = 0;
    virtual topology primitive_topology() const = 0;
};

class line2D : public drawable2D
{
  public:
    line2D(const glm::vec2 &p1, const glm::vec2 &p2);

    void draw(render_system2D &rs) const override;
    topology primitive_topology() const override;
};
} // namespace lynx

#endif