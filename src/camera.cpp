#include "lynx/pch.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
camera3D::camera3D(const glm::vec3 &orthographic_dimensions)
{
    orthographic_projection(orthographic_dimensions);
}
camera3D::camera3D(const glm::vec3 &min, const glm::vec3 &max)
{
    orthographic_projection(min, max);
}
camera3D::camera3D(const float fovy, const float aspect, const float near, const float far)
{
    perspective_projection(fovy, aspect, near, far);
}

void camera3D::orthographic_projection(const glm::vec3 &dimensions)
{
    orthographic_projection(-0.5f * dimensions, 0.5f * dimensions);
}
void camera3D::orthographic_projection(const glm::vec3 &min, const glm::vec3 &max)
{
    m_projection = glm::mat4{1.0f};
    m_projection[0][0] = 2.f / (max.x - min.x);
    m_projection[1][1] = 2.f / (max.y - min.y);
    m_projection[2][2] = 1.f / (max.z - min.z);
    m_projection[3][0] = -(max.x + min.x) / (max.x - min.x);
    m_projection[3][1] = -(max.y + min.y) / (max.y - min.y);
    m_projection[3][2] = -min.z / (max.z - min.z);
}
void camera3D::perspective_projection(const float fovy, const float aspect, const float near, const float far)
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

void camera3D::view_as_direction(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &up)
{
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    m_view = glm::mat4{1.f};
    m_view[0][0] = u.x;
    m_view[1][0] = u.y;
    m_view[2][0] = u.z;
    m_view[0][1] = v.x;
    m_view[1][1] = v.y;
    m_view[2][1] = v.z;
    m_view[0][2] = w.x;
    m_view[1][2] = w.y;
    m_view[2][2] = w.z;
    m_view[3][0] = -glm::dot(u, position);
    m_view[3][1] = -glm::dot(v, position);
    m_view[3][2] = -glm::dot(w, position);
}
void camera3D::view_as_target(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
{
    view_as_direction(position, target - position, up);
}
void camera3D::view_as_transform(const glm::vec3 &position, const glm::vec3 &rotation)
{
    const float c3 = cosf(rotation.z);
    const float s3 = sinf(rotation.z);
    const float c2 = cosf(rotation.x);
    const float s2 = sinf(rotation.x);
    const float c1 = cosf(rotation.y);
    const float s1 = sinf(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    m_view = glm::mat4{1.f};
    m_view[0][0] = u.x;
    m_view[1][0] = u.y;
    m_view[2][0] = u.z;
    m_view[0][1] = v.x;
    m_view[1][1] = v.y;
    m_view[2][1] = v.z;
    m_view[0][2] = w.x;
    m_view[1][2] = w.y;
    m_view[2][2] = w.z;
    m_view[3][0] = -glm::dot(u, position);
    m_view[3][1] = -glm::dot(v, position);
    m_view[3][2] = -glm::dot(w, position);
}

const glm::mat4 &camera3D::projection_matrix() const
{
    return m_projection;
}

const glm::mat4 &camera3D::view_matrix() const
{
    return m_view;
}
} // namespace lynx