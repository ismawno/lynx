#pragma once

#include "kit/utility/transform.hpp"

namespace lynx::dimension
{
struct two;
struct three;
} // namespace lynx::dimension

namespace lynx
{
template <typename Dim>
concept Dimension = std::is_same_v<Dim, dimension::two> || std::is_same_v<Dim, dimension::three>;

template <Dimension Dim> class camera;
template <Dimension Dim> class window;
template <Dimension Dim> class render_system;
template <Dimension Dim> class model;
template <Dimension Dim> class shape;

class shape2D;
class model3D;
class camera3D;

} // namespace lynx

namespace lynx::dimension
{
struct two
{
    static inline constexpr std::uint32_t N = 2;
    using transform_t = kit::transform2D<float>;
    using camera_t = camera<two>;
    using shape_t = shape2D;
    using model_t = model<two>;
};
struct three
{
    static inline constexpr std::uint32_t N = 3;
    using transform_t = kit::transform3D<float>;
    using camera_t = camera3D;
    using shape_t = shape<three>;
    using model_t = model3D;
};
} // namespace lynx::dimension
