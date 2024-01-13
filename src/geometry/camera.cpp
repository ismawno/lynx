#include "lynx/internal/pch.hpp"
#include "lynx/geometry/camera.hpp"
#include "kit/profile/perf.hpp"

namespace lynx
{
template <Dimension Dim> const glm::mat4 &camera<Dim>::projection() const
{
    return m_projection;
}
template <Dimension Dim> const glm::mat4 &camera<Dim>::inverse_projection() const
{
    return m_inv_projection;
}
template <Dimension Dim> void camera<Dim>::flip_y_axis()
{
    m_y_flipped = !m_y_flipped;
}

template <Dimension Dim> void camera<Dim>::keep_aspect_ratio(const float aspect)
{
    transform.scale.x = aspect * transform.scale.y;
}

template <Dimension Dim> typename camera<Dim>::vec_t camera<Dim>::screen_to_world(const glm::vec2 &screen_pos) const
{
    const glm::vec4 pos4 = m_inv_projection * glm::vec4(screen_pos, 0.5f, 1.f);
    return vec_t(pos4) * pos4.w;
}

template <Dimension Dim> glm::vec2 camera<Dim>::world_to_screen(const vec_t &world_pos) const
{
    if constexpr (std::is_same_v<Dim, dimension::two>)
    {
        const glm::vec4 pos4 = m_projection * glm::vec4(world_pos, 0.5f, 1.f);
        return glm::vec2(pos4) / pos4.w;
    }
    else
    {
        const glm::vec4 pos4 = m_projection * glm::vec4(world_pos, 1.f);
        return glm::vec2(pos4) / pos4.w;
    }
}

void camera3D::point_towards(const glm::vec3 &direction)
{
    const float roty = atan2f(direction.x, direction.z);
    const float rotx = -atan2f(direction.y, direction.z * cosf(roty) + direction.x * sinf(roty));
    transform.rotation = kit::transform3D<float>::YX(roty, rotx);
}
void camera3D::point_to(const glm::vec3 &position)
{
    point_towards(position - transform.position);
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
    KIT_PERF_FUNCTION()
    if (m_y_flipped)
        transform.scale.y = -transform.scale.y;
    m_projection = transform.inverse_scale_center_rotate_translate4();
    m_inv_projection = transform.scale_center_rotate_translate4();
    if (m_y_flipped)
        transform.scale.y = -transform.scale.y;
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
    KIT_PERF_FUNCTION()
    if (m_y_flipped)
        transform.scale.y = -transform.scale.y;
    m_projection = transform.inverse_scale_center_rotate_translate4();
    m_inv_projection = transform.scale_center_rotate_translate4();
    if (m_y_flipped)
        transform.scale.y = -transform.scale.y;
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
    : near(near), far(far), fov(fovy), m_aspect(aspect)
{
    transform.position = position;
    transform.rotation = rotation;
    KIT_ASSERT_ERROR(aspect > 0.0f, "Aspect ratio must be greater than 0");
    KIT_ASSERT_ERROR(fov > 0.0f, "The tangent of the field of view must be greater than 0");
}

void perspective3D::keep_aspect_ratio(const float aspect)
{
    m_aspect = aspect;
}

void perspective3D::update_transformation_matrices()
{
    KIT_PERF_FUNCTION()
    if (m_y_flipped)
        transform.scale.y = -transform.scale.y;
    const float half_tan_fovy = tanf(0.5f * fov);

    glm::mat4 perspective = glm::mat4{0.0f};
    perspective[0][0] = 1.f / (m_aspect * half_tan_fovy);
    perspective[1][1] = 1.f / half_tan_fovy;
    perspective[2][2] = far / (far - near);
    perspective[2][3] = 1.f;
    perspective[3][2] = far * near / (near - far);

    glm::mat4 inv_perspective = glm::mat4{0.0f};
    inv_perspective[0][0] = m_aspect * half_tan_fovy;
    inv_perspective[1][1] = half_tan_fovy;
    inv_perspective[3][3] = 1.f / near;
    inv_perspective[3][2] = 1.f;
    inv_perspective[2][3] = (near - far) / (far * near);

    m_projection = perspective * transform.inverse_scale_center_rotate_translate4();
    m_inv_projection = transform.scale_center_rotate_translate4() * inv_perspective;

    if (m_y_flipped)
        transform.scale.y = -transform.scale.y;
}

template class camera<dimension::two>;
template class camera<dimension::three>;
} // namespace lynx