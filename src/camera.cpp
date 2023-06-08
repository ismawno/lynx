#include "lynx/pch.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
camera::camera(const glm::vec3 &orthographic_dimensions)
{
    orthographic_projection(orthographic_dimensions);
}
camera::camera(const glm::vec3 &min, const glm::vec3 &max)
{
    orthographic_projection(min, max);
}
camera::camera(const float fovy, const float aspect, const float near, const float far)
{
    perspective_projection(fovy, aspect, near, far);
}

void camera::orthographic_projection(const glm::vec3 &dimensions)
{
    orthographic_projection(-0.5f * dimensions, 0.5f * dimensions);
}
void camera::orthographic_projection(const glm::vec3 &min, const glm::vec3 &max)
{
    m_projection = glm::mat4{1.0f};
    m_projection[0][0] = 2.f / (max.x - min.x);
    m_projection[1][1] = 2.f / (max.y - min.y);
    m_projection[2][2] = 1.f / (max.z - min.z);
    m_projection[3][0] = -(max.x + min.x) / (max.x - min.x);
    m_projection[3][1] = -(max.y + min.y) / (max.y - min.y);
    m_projection[3][2] = -min.z / (max.z - min.z);
}
void camera::perspective_projection(const float fovy, const float aspect, const float near, const float far)
{
    DBG_ASSERT_ERROR(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f,
                     "Aspect ratio must be greater than 0");
    const float tan_half = tanf(fovy / 2.f);
    DBG_ASSERT_ERROR(glm::abs(tan_half - std::numeric_limits<float>::epsilon()) > 0.0f,
                     "The tangent of the field of view must not be 0");

    m_projection = glm::mat4{0.0f};
    m_projection[0][0] = 1.f / (aspect * tan_half);
    m_projection[1][1] = 1.f / (tan_half);
    m_projection[2][2] = far / (far - near);
    m_projection[2][3] = 1.f;
    m_projection[3][2] = -(far * near) / (far - near);
}

const glm::mat4 &camera::projection_matrix() const
{
    return m_projection;
}
} // namespace lynx