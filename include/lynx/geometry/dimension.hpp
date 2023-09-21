#include "kit/utility/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace lynx
{
template <typename Dim> struct vertex;

template <typename Dim> class camera;

class camera3D;
} // namespace lynx

namespace lynx::dimension
{
struct two
{
    using vec_t = glm::vec2;
    using transform_t = kit::transform2D;
    using vertex_t = vertex<two>;
    using camera_t = camera<two>;
};
struct three
{
    using vec_t = glm::vec3;
    using transform_t = kit::transform3D;
    using vertex_t = vertex<three>;
    using camera_t = camera3D;
};
} // namespace lynx::dimension