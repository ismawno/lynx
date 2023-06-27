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

void orthographic2D::update_transformation_matrices()
{
    m_projection = transform.inverse_scale_rotate_translate();
    m_inv_projection = transform.scale_rotate_translate();
}

void camera3D::keep_aspect_ratio(const float aspect)
{
    transform.scale.x = aspect * transform.scale.y;
}

glm::vec3 camera3D::screen_to_world(const glm::vec2 &screen_pos, const float z_screen) const
{
    DBG_ASSERT_ERROR(z_screen >= 0.f && z_screen <= 1.f,
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
    const float yrot = atan2f(direction.x, direction.z);
    transform.rotation.y = yrot;
    transform.rotation.x = -atan2f(direction.y, direction.z * cosf(yrot) + direction.x * sinf(yrot));
}
void camera3D::point_to(const glm::vec3 &position)
{
    point_towards(position - transform.position);
}

orthographic3D::orthographic3D(float aspect, float xy_size, float z_size, const glm::vec3 &rotation)
    : orthographic3D(glm::vec3(0.f), aspect, xy_size, z_size, rotation)
{
}
orthographic3D::orthographic3D(const glm::vec3 &position, float aspect, float xy_size, float z_size,
                               const glm::vec3 &rotation)
    : orthographic3D(position, {aspect * xy_size, xy_size, z_size}, rotation)
{
}

orthographic3D::orthographic3D(const glm::vec3 &size, const glm::vec3 &rotation)
    : orthographic3D(glm::vec3(0.f), size, rotation)
{
}
orthographic3D::orthographic3D(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &rotation)
{
    transform.position = position;
    transform.scale = size;
    transform.rotation = rotation;
}

void orthographic3D::update_transformation_matrices()
{
    m_projection = transform.inverse_scale_rotate_translate();
    m_inv_projection = transform.scale_rotate_translate();
}

perspective3D::perspective3D(const float aspect, const float fovy, const glm::vec3 &rotation, const float near,
                             const float far)
    : perspective3D(glm::vec3(0.f), aspect, fovy, rotation, near, far)
{
}
perspective3D::perspective3D(const glm::vec3 &position, const float aspect, const float fovy, const glm::vec3 &rotation,
                             const float near, const float far)
    : m_near(near), m_far(far)
{
    transform.position = position;
    transform.rotation = rotation;
    fov(aspect, fovy);
    DBG_ASSERT_ERROR(aspect > 0.0f, "Aspect ratio must be greater than 0");
    DBG_ASSERT_ERROR(fov() > 0.0f, "The tangent of the field of view must be greater than 0");
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
    return 2.f * atanf(transform.scale.y / m_near);
}

void perspective3D::near(const float near)
{
    m_near = near;
}
void perspective3D::far(const float far)
{
    m_far = far;
}
void perspective3D::fov(const float aspect, const float fovy)
{
    const float tan_half = tanf(0.5f * fovy);
    transform.scale.x = m_near * aspect * tan_half;
    transform.scale.y = m_near * tan_half;
}

void perspective3D::update_transformation_matrices()
{
    update_projection();
    update_view();
}

void perspective3D::update_projection()
{
    m_projection = glm::mat4{0.0f};
    m_projection[0][0] = m_near / transform.scale.x;
    m_projection[1][1] = m_near / transform.scale.y;
    m_projection[2][2] = m_far / (m_far - m_near);
    m_projection[2][3] = 1.f;
    m_projection[3][2] = m_far * m_near / (m_near - m_far);

    m_inv_projection = glm::mat4{0.0f};
    m_inv_projection[0][0] = transform.scale.x / m_near;
    m_inv_projection[1][1] = transform.scale.y / m_near;
    m_inv_projection[3][3] = 1.f / m_near;
    m_inv_projection[3][2] = 1.f;
    m_inv_projection[2][3] = (m_near - m_far) / (m_far * m_near);
}

void perspective3D::update_view()
{
    const float c3 = cosf(transform.rotation.z);
    const float s3 = sinf(transform.rotation.z);
    const float c2 = cosf(transform.rotation.x);
    const float s2 = sinf(transform.rotation.x);
    const float c1 = cosf(transform.rotation.y);
    const float s1 = sinf(transform.rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};

    glm::mat4 view = glm::mat4{1.f};
    view[0][0] = u.x;
    view[1][0] = u.y;
    view[2][0] = u.z;
    view[0][1] = v.x;
    view[1][1] = v.y;
    view[2][1] = v.z;
    view[0][2] = w.x;
    view[1][2] = w.y;
    view[2][2] = w.z;
    view[3][0] = -glm::dot(u, transform.position);
    view[3][1] = -glm::dot(v, transform.position);
    view[3][2] = -glm::dot(w, transform.position);

    glm::mat4 inv_view = glm::mat4{1.f};
    inv_view[0][0] = u.x;
    inv_view[1][0] = v.x;
    inv_view[2][0] = w.x;
    inv_view[0][1] = u.y;
    inv_view[1][1] = v.y;
    inv_view[2][1] = w.y;
    inv_view[0][2] = u.z;
    inv_view[1][2] = v.z;
    inv_view[2][2] = w.z;
    inv_view[3][0] = transform.position.x;
    inv_view[3][1] = transform.position.y;
    inv_view[3][2] = transform.position.z;

    m_projection = m_projection * view;
    m_inv_projection = inv_view * m_inv_projection;
}
} // namespace lynx