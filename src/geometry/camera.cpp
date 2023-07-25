#include "lynx/internal/pch.hpp"
#include "lynx/geometry/camera.hpp"

namespace lynx
{
const glm::mat4 &camera::projection() const
{
    return m_projection;
}

const glm::mat4 &camera::inverse_projection() const
{
    return m_inv_projection;
}

void camera2D::keep_aspect_ratio(const float aspect)
{
    transform.scale.x = aspect * transform.scale.y;
}

glm::vec2 camera2D::screen_to_world(const glm::vec2 &screen_pos) const
{
    const glm::vec4 pos4 = m_inv_projection * glm::vec4(screen_pos, 0.5f, 1.f);
    return glm::vec2(pos4) * pos4.w;
}

glm::vec2 camera2D::world_to_screen(const glm::vec2 &world_pos) const
{
    const glm::vec4 pos4 = m_projection * glm::vec4(world_pos, 0.5f, 1.f);
    return glm::vec2(pos4) / pos4.w;
}

orthographic2D::orthographic2D(const float aspect, const float size, const float rotation)
    : orthographic2D({0.f, 0.f}, aspect, size, rotation)
{
}

orthographic2D::orthographic2D(const glm::vec2 &position, const float aspect, const float size, const float rotation)
    : orthographic2D(position, {size * aspect, size}, rotation)
{
}

orthographic2D::orthographic2D(const glm::vec2 &size, const float rotation) : orthographic2D({0.f, 0.f}, size, rotation)
{
}

orthographic2D::orthographic2D(const glm::vec2 &position, const glm::vec2 &size, const float rotation)
{
    transform.position = position;
    transform.scale = size;
    transform.rotation = rotation;
}

float orthographic2D::size() const
{
    return transform.scale.y;
}
void orthographic2D::size(const float size)
{
    const float aspect = transform.scale.x / transform.scale.y;
    transform.scale.y = size;
    transform.scale.x = aspect * size;
}

void orthographic2D::update_transformation_matrices()
{
    m_projection = transform.transform_as_camera();
    m_inv_projection = transform.inverse_as_camera();
}

void camera3D::keep_aspect_ratio(const float aspect)
{
    transform.scale.x = aspect * glm::abs(transform.scale.y);
}

glm::vec3 camera3D::screen_to_world(const glm::vec2 &screen_pos, const float z_screen) const
{
    KIT_ASSERT_ERROR(z_screen >= 0.f && z_screen <= 1.f,
                     "Value of z-screen must be normalized ([0, 1]) to fit into vulkan's canonical volume. z: {0}",
                     z_screen)
    const glm::vec4 pos4 = m_inv_projection * glm::vec4(screen_pos, z_screen, 1.f);
    return glm::vec3(pos4) * pos4.w;
}

glm::vec2 camera3D::world_to_screen(const glm::vec3 &world_pos) const
{
    const glm::vec4 pos4 = m_projection * glm::vec4(world_pos, 1.f);
    return glm::vec2(pos4) / pos4.w;
}

void camera3D::point_towards(const glm::vec3 &direction)
{
    const float roty = atan2f(direction.x, direction.z);
    const float rotx = -atan2f(direction.y, direction.z * cosf(roty) + direction.x * sinf(roty));
    transform.rotation = transform3D::YX(roty, rotx);
}
void camera3D::point_to(const glm::vec3 &position)
{
    point_towards(position - transform.position);
}

orthographic3D::orthographic3D(float aspect, float xy_size, float z_size, const glm::mat3 &rotation)
    : orthographic3D(glm::vec3(0.f), aspect, xy_size, z_size, rotation)
{
}
orthographic3D::orthographic3D(const glm::vec3 &position, float aspect, float xy_size, float z_size,
                               const glm::mat3 &rotation)
    : orthographic3D(position, {aspect * xy_size, xy_size, z_size}, rotation)
{
}

orthographic3D::orthographic3D(const glm::vec3 &size, const glm::mat3 &rotation)
    : orthographic3D(glm::vec3(0.f), size, rotation)
{
}
orthographic3D::orthographic3D(const glm::vec3 &position, const glm::vec3 &size, const glm::mat3 &rotation)
{
    transform.position = position;
    transform.scale = size;
    transform.rotation = rotation;
}

void orthographic3D::update_transformation_matrices()
{
    m_projection = transform.transform_as_camera();
    m_inv_projection = transform.inverse_as_camera();
}

float orthographic3D::size() const
{
    return transform.scale.y;
}
void orthographic3D::size(const float size)
{
    const float aspect = transform.scale.x / transform.scale.y;
    transform.scale.y = size;
    transform.scale.x = aspect * size;
}

perspective3D::perspective3D(const float aspect, const float fovy, const glm::mat3 &rotation, const float near,
                             const float far)
    : perspective3D(glm::vec3(0.f), aspect, fovy, rotation, near, far)
{
}
perspective3D::perspective3D(const glm::vec3 &position, const float aspect, const float fovy, const glm::mat3 &rotation,
                             const float near, const float far)
    : m_near(near), m_far(far), m_fov(fovy), m_half_tan_fovy(tanf(0.5f * fovy)), m_aspect(aspect)
{
    transform.position = position;
    transform.rotation = rotation;
    KIT_ASSERT_ERROR(aspect > 0.0f, "Aspect ratio must be greater than 0");
    KIT_ASSERT_ERROR(m_fov > 0.0f, "The tangent of the field of view must be greater than 0");
}

float perspective3D::near() const
{
    return m_near;
}
float perspective3D::far() const
{
    return m_far;
}
float perspective3D::fov() const
{
    return m_fov;
}

void perspective3D::near(const float near)
{
    m_near = near;
}
void perspective3D::far(const float far)
{
    m_far = far;
}
void perspective3D::fov(const float fovy)
{
    m_fov = fovy;
    m_half_tan_fovy = tanf(0.5f * fovy);
}

void perspective3D::keep_aspect_ratio(const float aspect)
{
    m_aspect = aspect;
}

void perspective3D::update_transformation_matrices()
{
    glm::mat4 perspective = glm::mat4{0.0f};
    perspective[0][0] = 1.f / (m_aspect * m_half_tan_fovy);
    perspective[1][1] = 1.f / m_half_tan_fovy;
    perspective[2][2] = m_far / (m_far - m_near);
    perspective[2][3] = 1.f;
    perspective[3][2] = m_far * m_near / (m_near - m_far);

    glm::mat4 inv_perspective = glm::mat4{0.0f};
    inv_perspective[0][0] = m_aspect * m_half_tan_fovy;
    inv_perspective[1][1] = m_half_tan_fovy;
    inv_perspective[3][3] = 1.f / m_near;
    inv_perspective[3][2] = 1.f;
    inv_perspective[2][3] = (m_near - m_far) / (m_far * m_near);

    m_projection = perspective * transform.transform_as_camera();
    m_inv_projection = transform.inverse_as_camera() * inv_perspective;
}
} // namespace lynx