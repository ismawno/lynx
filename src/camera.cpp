#include "lynx/pch.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
const glm::mat4 &camera::projection() const
{
    return m_projection;
}
const glm::mat4 &camera::view() const
{
    return m_view;
}

void camera2D::keep_aspect_ratio(const float aspect)
{
    transform.scale.x = aspect * transform.scale.y;
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
    update_projection();
    update_view();
}

void orthographic2D::update_projection()
{
    DBG_ASSERT_CRITICAL(transform.scale.x > 0.f && transform.scale.y > 0.f,
                        "Scale transform values must be greater than 0")
    m_projection = glm::mat4{1.f};
    m_projection[0][0] = 2.f / transform.scale.x;
    m_projection[1][1] = 2.f / transform.scale.y;
}

void orthographic2D::update_view()
{
    const float c = cosf(transform.rotation);
    const float s = sinf(transform.rotation);

    m_view = glm::mat4{1.f};
    m_view[0][0] = c;
    m_view[1][0] = s;
    m_view[0][1] = -s;
    m_view[1][1] = c;

    const glm::vec2 u{c, s};
    const glm::vec2 v{-s, c};
    m_view[3][0] = -glm::dot(u, transform.position);
    m_view[3][1] = -glm::dot(v, transform.position);
    //[3][2] may need to be non zero
}

void camera3D::keep_aspect_ratio(const float aspect)
{
    transform.scale.x = aspect * transform.scale.y;
}

void camera3D::point_towards(const glm::vec3 &direction)
{
    float &roty = transform.rotation.y;
    roty = atan2f(direction.x, direction.z);
    transform.rotation.x = -atan2f(direction.y, direction.z * cosf(roty) + direction.x * sinf(roty));
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
    update_projection();
    update_view();
}

void orthographic3D::update_projection()
{
    DBG_ASSERT_CRITICAL(transform.scale.x > 0.f && transform.scale.y > 0.f && transform.scale.z > 0.f,
                        "Scale transform values must be greater than 0")
    m_projection = glm::mat4{1.0f};
    m_projection[0][0] = 2.f / transform.scale.x;
    m_projection[1][1] = 2.f / transform.scale.y;
    m_projection[2][2] = 1.f / transform.scale.z;
}

void orthographic3D::update_view()
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
    m_view[3][0] = -glm::dot(u, transform.position);
    m_view[3][1] = -glm::dot(v, transform.position);
    m_view[3][2] = -glm::dot(w, transform.position);
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
    m_projection[3][2] = -(m_far * m_near) / (m_far - m_near);
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
    m_view[3][0] = -glm::dot(u, transform.position);
    m_view[3][1] = -glm::dot(v, transform.position);
    m_view[3][2] = -glm::dot(w, transform.position);
}

// camera3D::camera3D(const glm::vec3 &orthographic_dimensions)
// {
//     orthographic_projection(orthographic_dimensions);
// }
// camera3D::camera3D(const glm::vec3 &min, const glm::vec3 &max)
// {
//     orthographic_projection(min, max);
// }
// camera3D::camera3D(const float fovy, const float aspect, const float near, const float far)
// {
//     perspective_projection(fovy, aspect, near, far);
// }

// void camera3D::orthographic_projection(const glm::vec3 &dimensions)
// {
//     orthographic_projection(-0.5f * dimensions, 0.5f * dimensions);
// }
// void camera3D::orthographic_projection(const glm::vec3 &min, const glm::vec3 &max)
// {
//     m_projection = glm::mat4{1.0f};
//     m_projection[0][0] = 2.f / (max.x - min.x);
//     m_projection[1][1] = 2.f / (max.y - min.y);
//     m_projection[2][2] = 1.f / (max.z - min.z);
//     m_projection[3][0] = -(max.x + min.x) / (max.x - min.x);
//     m_projection[3][1] = -(max.y + min.y) / (max.y - min.y);
//     m_projection[3][2] = -min.z / (max.z - min.z);
// }
// void camera3D::perspective_projection(const float fovy, const float aspect, const float near, const float far)
// {
//     DBG_ASSERT_ERROR(aspect > 0.0f, "Aspect ratio must be greater than 0");
//     const float tan_half = tanf(fovy / 2.f);
//     DBG_ASSERT_ERROR(tan_half > 0.0f, "The tangent of the field of view must be greater than 0");

//     m_projection = glm::mat4{0.0f};
//     m_projection[0][0] = 1.f / (aspect * tan_half);
//     m_projection[1][1] = 1.f / (tan_half);
//     m_projection[2][2] = far / (far - near);
//     m_projection[2][3] = 1.f;
//     m_projection[3][2] = -(far * near) / (far - near);
// }

// void camera3D::view_as_direction(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &up)
// {
//     const glm::vec3 w{glm::normalize(direction)};
//     const glm::vec3 u{glm::normalize(glm::cross(w, up))};
//     const glm::vec3 v{glm::cross(w, u)};

//     m_view = glm::mat4{1.f};
//     m_view[0][0] = u.x;
//     m_view[1][0] = u.y;
//     m_view[2][0] = u.z;
//     m_view[0][1] = v.x;
//     m_view[1][1] = v.y;
//     m_view[2][1] = v.z;
//     m_view[0][2] = w.x;
//     m_view[1][2] = w.y;
//     m_view[2][2] = w.z;
//     m_view[3][0] = -glm::dot(u, position);
//     m_view[3][1] = -glm::dot(v, position);
//     m_view[3][2] = -glm::dot(w, position);
// }
// void camera3D::view_as_target(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
// {
//     view_as_direction(position, target - position, up);
// }
// void camera3D::view_as_transform(const glm::vec3 &position, const glm::vec3 &rotation)
// {
//     const float c3 = cosf(rotation.z);
//     const float s3 = sinf(rotation.z);
//     const float c2 = cosf(rotation.x);
//     const float s2 = sinf(rotation.x);
//     const float c1 = cosf(rotation.y);
//     const float s1 = sinf(rotation.y);
//     const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
//     const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
//     const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
//     m_view = glm::mat4{1.f};
//     m_view[0][0] = u.x;
//     m_view[1][0] = u.y;
//     m_view[2][0] = u.z;
//     m_view[0][1] = v.x;
//     m_view[1][1] = v.y;
//     m_view[2][1] = v.z;
//     m_view[0][2] = w.x;
//     m_view[1][2] = w.y;
//     m_view[2][2] = w.z;
//     m_view[3][0] = -glm::dot(u, position);
//     m_view[3][1] = -glm::dot(v, position);
//     m_view[3][2] = -glm::dot(w, position);
// }

// const glm::mat4 &camera3D::projection_matrix() const
// {
//     return m_projection;
// }

// const glm::mat4 &camera3D::view_matrix() const
// {
//     return m_view;
// }
// } // namespace lynx
} // namespace lynx