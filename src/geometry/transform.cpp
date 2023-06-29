#include "lynx/internal/pch.hpp"
#include "lynx/geometry/transform.hpp"

namespace lynx
{
glm::mat4 transform2D::transform() const
{
    const auto [c, s] = trigonometric_functions(rotation);
    const glm::vec2 u = glm::vec2(c, -s) * scale;
    const glm::vec2 v = glm::vec2(s, c) * scale;
    const glm::vec2 t = position - glm::vec2(glm::dot(u, origin), glm::dot(v, origin));

    return glm::mat4{{
                         u.x,
                         v.x,
                         0.f,
                         0.f,
                     },
                     {
                         u.y,
                         v.y,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                     },
                     {t.x, t.y, 0.f, 1.f}};
}
glm::mat4 transform2D::inverse() const
{
    const auto [c, s] = trigonometric_functions(rotation);
    const glm::vec2 iu = glm::vec2(c, s) / scale.x;
    const glm::vec2 iv = glm::vec2(-s, c) / scale.y;
    const glm::vec2 it = origin - glm::vec2(glm::dot(iu, position), glm::dot(iv, position));

    return glm::mat4{{
                         iu.x,
                         iv.x,
                         0.f,
                         0.f,
                     },
                     {
                         iu.y,
                         iv.y,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                     },
                     {it.x, it.y, 0.f, 1.f}};
}

glm::mat4 transform2D::transform_as_camera() const
{
    const auto [c, s] = trigonometric_functions(rotation);
    const glm::vec2 u = glm::vec2(c, s);
    const glm::vec2 v = glm::vec2(-s, c);
    const glm::vec2 t = (origin - glm::vec2(glm::dot(u, position), glm::dot(v, position))) / scale;

    return glm::mat4{{
                         u.x / scale.x,
                         v.x / scale.y,
                         0.f,
                         0.f,
                     },
                     {
                         u.y / scale.x,
                         v.y / scale.y,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                     },
                     {t.x, t.y, 0.f, 1.f}};
}
glm::mat4 transform2D::inverse_as_camera() const
{
    const auto [c, s] = trigonometric_functions(rotation);
    const glm::vec2 iu = glm::vec2(c, -s);
    const glm::vec2 iv = glm::vec2(s, c);
    const glm::vec2 it = position - glm::vec2(glm::dot(iu, origin), glm::dot(iv, origin));

    return glm::mat4{{
                         iu.x * scale.x,
                         iv.x * scale.x,
                         0.f,
                         0.f,
                     },
                     {
                         iu.y * scale.y,
                         iv.y * scale.y,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                     },
                     {it.x, it.y, 0.f, 1.f}};
}

void transform2D::local_translate(const glm::vec2 &dpos)
{
    const auto [c, s] = trigonometric_functions(rotation);
    const glm::vec2 u{c, -s};
    const glm::vec2 v{s, c};
    position += glm::vec2(glm::dot(u, dpos), glm::dot(v, dpos));
}
void transform2D::xlocal_translate(float dx)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += glm::vec2(c * dx, s * dx);
}
void transform2D::ylocal_translate(float dy)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += glm::vec2(-s * dy, c * dy);
}

transform2D::trigonometry transform2D::trigonometric_functions(const float rotation)
{
    return {cosf(rotation), sinf(rotation)};
}

glm::mat4 transform3D::transform() const // YXZ
{
    auto [u, v, w] = rotation_basis(rotation);
    u *= scale;
    v *= scale;
    w *= scale;
    const glm::vec3 t = position - glm::vec3(glm::dot(u, origin), glm::dot(v, origin), glm::dot(w, origin));

    return glm::mat4{{
                         u.x,
                         v.x,
                         w.x,
                         0.f,
                     },
                     {
                         u.y,
                         v.y,
                         w.y,
                         0.f,
                     },
                     {
                         u.z,
                         v.z,
                         w.z,
                         0.f,
                     },
                     {t.x, t.y, t.z, 1.f}};
}

glm::mat4 transform3D::inverse() const // YXZ
{
    auto [iu, iv, iw] = inverse_rotation_basis(rotation);
    iu /= scale.x;
    iv /= scale.y;
    iw /= scale.z;
    const glm::vec3 it = origin - glm::vec3(glm::dot(iu, position), glm::dot(iv, position), glm::dot(iw, position));

    return glm::mat4{{
                         iu.x,
                         iv.x,
                         iw.x,
                         0.f,
                     },
                     {
                         iu.y,
                         iv.y,
                         iw.y,
                         0.f,
                     },
                     {
                         iu.z,
                         iv.z,
                         iw.z,
                         0.f,
                     },
                     {it.x, it.y, it.z, 1.f}};
}

glm::mat4 transform3D::transform_as_camera() const
{
    auto [u, v, w] = inverse_rotation_basis(rotation);
    const glm::vec3 it =
        (origin - glm::vec3(glm::dot(u, position), glm::dot(v, position), glm::dot(w, position))) / scale;

    return glm::mat4{{
                         u.x / scale.x,
                         v.x / scale.y,
                         w.x / scale.z,
                         0.f,
                     },
                     {
                         u.y / scale.x,
                         v.y / scale.y,
                         w.y / scale.z,
                         0.f,
                     },
                     {
                         u.z / scale.x,
                         v.z / scale.y,
                         w.z / scale.z,
                         0.f,
                     },
                     {it.x, it.y, it.z, 1.f}};
}
glm::mat4 transform3D::inverse_as_camera() const
{
    auto [u, v, w] = rotation_basis(rotation);
    const glm::vec3 t = position - glm::vec3(glm::dot(u, origin), glm::dot(v, origin), glm::dot(w, origin));

    return glm::mat4{{
                         u.x * scale.x,
                         v.x * scale.x,
                         w.x * scale.x,
                         0.f,
                     },
                     {
                         u.y * scale.y,
                         v.y * scale.y,
                         w.y * scale.y,
                         0.f,
                     },
                     {
                         u.z * scale.z,
                         v.z * scale.z,
                         w.z * scale.z,
                         0.f,
                     },
                     {t.x, t.y, t.z, 1.f}};
}

void transform3D::local_translate(const glm::vec3 &dpos)
{
    const auto [u, v, w] = rotation_basis(rotation);
    position += glm::vec3(glm::dot(u, dpos), glm::dot(v, dpos), glm::dot(w, dpos));
}
void transform3D::xlocal_translate(float dx)
{
    const auto [u, v, w] = rotation_basis(rotation);
    position += glm::vec3(u.x * dx, v.x * dx, w.x * dx);
}
void transform3D::ylocal_translate(float dy)
{
    const auto [u, v, w] = rotation_basis(rotation);
    position += glm::vec3(u.y * dy, v.y * dy, w.y * dy);
}
void transform3D::zlocal_translate(float dz)
{
    const auto [u, v, w] = rotation_basis(rotation);
    position += glm::vec3(u.z * dz, v.z * dz, w.z * dz);
}

transform3D::trigonometry transform3D::trigonometric_functions(const glm::vec3 &rotation)
{
    return {cosf(rotation.y), sinf(rotation.y), cosf(rotation.x), sinf(rotation.x), cosf(rotation.z), sinf(rotation.z)};
}

transform3D::rbasis transform3D::rotation_basis(const glm::vec3 &rotation)
{
    const auto [c1, s1, c2, s2, c3, s3] = trigonometric_functions(rotation);
    return {{(c1 * c3 + s1 * s2 * s3), (c3 * s1 * s2 - c1 * s3), (c2 * s1)},
            {(c2 * s3), (c2 * c3), (-s2)},
            {(c1 * s2 * s3 - c3 * s1), (c1 * c3 * s2 + s1 * s3), (c1 * c2)}};
}

transform3D::rbasis transform3D::inverse_rotation_basis(const glm::vec3 &rotation)
{
    const auto [c1, s1, c2, s2, c3, s3] = trigonometric_functions(rotation);
    return {{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)},
            {(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)},
            {(c2 * s1), (-s2), (c1 * c2)}};
}
} // namespace lynx