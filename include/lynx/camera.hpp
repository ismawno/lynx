#ifndef LYNX_CAMERA_HPP
#define LYNX_CAMERA_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lynx
{
class camera
{
  public:
    camera() = default;
    camera(const glm::vec3 &orthographic_dimensions);
    camera(const glm::vec3 &min, const glm::vec3 &max);
    camera(float fovy, float aspect, float near, float far);

    void orthographic_projection(const glm::vec3 &dimensions);
    void orthographic_projection(const glm::vec3 &min, const glm::vec3 &max);
    void perspective_projection(float fovy, float aspect, float near, float far);

    const glm::mat4 &projection_matrix() const;

  private:
    glm::mat4 m_projection{1.f};
};
} // namespace lynx

#endif