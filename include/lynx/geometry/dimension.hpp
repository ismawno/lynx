#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace lynx::dimension
{
struct two
{
    using vec_t = glm::vec2;
};
struct three
{
    using vec_t = glm::vec3;
};
} // namespace lynx::dimension