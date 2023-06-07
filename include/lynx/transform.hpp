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
    glm::vec2 translation{0.f};
    glm::vec2 scale{1.f};
    float rotation = 0.f;
    operator glm::mat4() const;
};

struct transform3D
{
    glm::vec3 translation{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 rotation{0.f};
    operator glm::mat4() const;
};
} // namespace lynx

#endif