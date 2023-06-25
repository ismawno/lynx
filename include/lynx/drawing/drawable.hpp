#ifndef LYNX_DRAWABLE_HPP
#define LYNX_DRAWABLE_HPP

#include "lynx/internal/core.hpp"

namespace lynx
{
class window2D;
class window3D;

class render_system2D;
class render_system3D;

enum topology
{
    POINT_LIST = 0,
    LINE_LIST = 1,
    LINE_STRIP = 2,
    TRIANGLE_LIST = 3,
    TRIANGLE_STRIP = 4
};

class drawable2D
{
  public:
    virtual ~drawable2D() = default;

    virtual void draw(window2D &win) const = 0;
    virtual void draw(render_system2D &rs) const
    {
        DBG_ERROR("To draw to an arbitrary render system, the draw render system overload must be overriden")
    }
};

class drawable3D
{
  public:
    virtual ~drawable3D() = default;

    virtual void draw(window3D &win) const = 0;
    virtual void draw(render_system3D &rs) const
    {
        DBG_ERROR("To draw to an arbitrary render system, the draw render system overload must be overriden")
    }
};
} // namespace lynx

#endif