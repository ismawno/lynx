#ifndef LYNX_TRANSFORM_HPP
#define LYNX_TRANSFORM_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lynx
{
struct transform2D
{
    glm::vec2 position{0.f};
    glm::vec2 scale{1.f};
    float rotation = 0.f;
    operator glm::mat4() const;

    static void reset_z_offset_counter();

  private:
    inline static std::size_t s_z_offset_counter = 0;
};

struct transform3D
{
    glm::vec3 position{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 rotation{0.f};
    operator glm::mat4() const;
};
} // namespace lynx

#endif