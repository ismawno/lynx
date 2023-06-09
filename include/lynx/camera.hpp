#ifndef LYNX_CAMERA_HPP
#define LYNX_CAMERA_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lynx
{
class camera3D
{
  public:
    camera3D() = default;
    camera3D(const glm::vec3 &orthographic_dimensions);
    camera3D(const glm::vec3 &min, const glm::vec3 &max);
    camera3D(float fovy, float aspect, float near, float far);

    // void ortho
    void orthographic_projection(const glm::vec3 &dimensions);
    void orthographic_projection(const glm::vec3 &min, const glm::vec3 &max);
    void perspective_projection(float fovy, float aspect, float near, float far);

    void view_as_direction(const glm::vec3 &position, const glm::vec3 &direction,
                           const glm::vec3 &up = {0.f, -1.f, 0.f});
    void view_as_target(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up = {0.f, -1.f, 0.f});
    void view_as_transform(const glm::vec3 &position, const glm::vec3 &rotation);

    const glm::mat4 &projection_matrix() const;
    const glm::mat4 &view_matrix() const;

  private:
    glm::mat4 m_projection{1.f};
    glm::mat4 m_view{1.f};
};
} // namespace lynx

#endif