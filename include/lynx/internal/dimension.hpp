#ifndef LYNX_DIMENSION_HPP
#define LYNX_DIMENSION_HPP

#include "kit/utility/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace lynx
{
template <typename Dim> class camera;
template <typename Dim> class window;
template <typename Dim> class render_system;
template <typename Dim> class model;
template <typename Dim> class shape;

class render_system2D;
class shape2D;
class model3D;
class camera3D;

} // namespace lynx

namespace lynx::dimension
{
struct two
{
    using vec_t = glm::vec2;
    using transform_t = kit::transform2D;
    using camera_t = camera<two>;
    using render_system_t = render_system2D;
    using shape_t = shape2D;
    using model_t = model<two>;
};
struct three
{
    using vec_t = glm::vec3;
    using transform_t = kit::transform3D;
    using camera_t = camera3D;
    using render_system_t = render_system<three>;
    using shape_t = shape<three>;
    using model_t = model3D;
};
} // namespace lynx::dimension
#endif