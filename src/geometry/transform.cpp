#include "lynx/internal/pch.hpp"
#include "lynx/geometry/transform.hpp"

namespace lynx
{
glm::mat4 transform2D::transform() const
{
    m_z_offset = 1.f - ++s_z_offset_counter * std::numeric_limits<float>::epsilon();

    const auto [c, s] = trigonometric_functions(m_rotation);
    const glm::vec2 u = glm::vec2(c, -s) * m_scale;
    const glm::vec2 v = glm::vec2(s, c) * m_scale;
    const glm::vec2 t = m_translation + m_origin - glm::vec2(glm::dot(u, m_origin), glm::dot(v, m_origin));

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
                     {t.x, t.y, m_z_offset, 1.f}};
}
glm::mat4 transform2D::inverse() const
{
    const auto [c, s] = trigonometric_functions(m_rotation);
    const glm::vec2 iu = glm::vec2(c, s) / m_scale.x;
    const glm::vec2 iv = glm::vec2(-s, c) / m_scale.y;
    const glm::vec2 displacement = m_translation + m_origin;
    const glm::vec2 it = m_origin - glm::vec2(glm::dot(iu, displacement), glm::dot(iv, displacement));

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
                     {it.x, it.y, -m_z_offset, 1.f}};
}

transform2D::trigonometry transform2D::trigonometric_functions(const float rotation)
{
    return {cosf(rotation), sinf(rotation)};
}

const glm::vec2 &transform2D::position() const
{
    return m_position;
}
const glm::vec2 &transform2D::translation() const
{
    return m_translation;
}
const glm::vec2 &transform2D::scale() const
{
    return m_scale;
}
const glm::vec2 &transform2D::origin() const
{
    return m_origin;
}
float transform2D::rotation() const
{
    return m_rotation;
}

void transform2D::position(const glm::vec2 &position)
{
    m_position = position;
    m_translation = position - m_origin;
}
void transform2D::translation(const glm::vec2 &translation)
{
    m_translation = translation;
    m_position = m_origin + m_translation;
}
void transform2D::scale(const glm::vec2 &scale)
{
    m_scale = scale;
}
void transform2D::origin(const glm::vec2 &origin)
{
    m_origin = origin;
    m_position = origin + m_translation;
}
void transform2D::rotation(const float rotation)
{
    m_rotation = rotation;
}

void transform2D::translate(const glm::vec2 &translation)
{
    m_translation += translation;
    m_position += translation;
}
void transform2D::stretch(const glm::vec2 &stretch)
{
    m_scale += stretch;
}

void transform2D::xposition(const float xp)
{
    m_position.x = xp;
    m_translation.x = xp - m_origin.x;
}
void transform2D::yposition(const float yp)
{
    m_position.y = yp;
    m_translation.y = yp - m_origin.y;
}

void transform2D::xtranslation(const float xt)
{
    m_translation.x = xt;
    m_position.x = m_origin.x + xt;
}
void transform2D::ytranslation(const float yt)
{
    m_translation.y = yt;
    m_position.y = m_origin.y + yt;
}

void transform2D::xtranslate(const float xt)
{
    m_translation.x += xt;
    m_position.x += xt;
}
void transform2D::ytranslate(const float yt)
{
    m_translation.y += yt;
    m_position.y += yt;
}

void transform2D::xscale(const float xs)
{
    m_scale.x = xs;
}
void transform2D::yscale(const float ys)
{
    m_scale.y = ys;
}

void transform2D::xstretch(const float xs)
{
    m_scale.x += xs;
}
void transform2D::ystretch(const float ys)
{
    m_scale.y += ys;
}

void transform2D::xorigin(const float xo)
{
    m_origin.x = xo;
    m_position.x = xo + m_translation.x;
}
void transform2D::yorigin(const float yo)
{
    m_origin.y = yo;
    m_position.y = yo + m_translation.y;
}

void transform2D::rotate(const float angle)
{
    m_rotation += angle;
}
void transform2D::reset_z_offset_counter()
{
    s_z_offset_counter = 0;
}

glm::mat4 transform3D::transform() const // YXZ
{
    auto [u, v, w] = rotation_basis(m_rotation);
    u *= m_scale;
    v *= m_scale;
    w *= m_scale;
    const glm::vec3 t =
        m_translation + m_origin - glm::vec3(glm::dot(u, m_origin), glm::dot(v, m_origin), glm::dot(w, m_origin));

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
    auto [iu, iv, iw] = inverse_rotation_basis(m_rotation);
    iu /= m_scale.x;
    iv /= m_scale.y;
    iw /= m_scale.z;
    const glm::vec3 displacement = m_translation + m_origin;
    const glm::vec3 it =
        m_origin - glm::vec3(glm::dot(iu, displacement), glm::dot(iv, displacement), glm::dot(iw, displacement));

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

transform3D::trigonometry transform3D::trigonometric_functions(const glm::vec3 &rotation)
{
    return {cosf(rotation.x), sinf(rotation.x), cosf(rotation.y), sinf(rotation.y), cosf(rotation.z), sinf(rotation.z)};
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

const glm::vec3 &transform3D::position() const
{
    return m_position;
}
const glm::vec3 &transform3D::translation() const
{
    return m_translation;
}
const glm::vec3 &transform3D::scale() const
{
    return m_scale;
}
const glm::vec3 &transform3D::origin() const
{
    return m_origin;
}
const glm::vec3 &transform3D::rotation() const
{
    return m_rotation;
}

void transform3D::position(const glm::vec3 &position)
{
    m_position = position;
    m_translation = position - m_origin;
}
void transform3D::translation(const glm::vec3 &translation)
{
    m_translation = translation;
    m_position = m_origin + m_translation;
}
void transform3D::scale(const glm::vec3 &scale)
{
    m_scale = scale;
}
void transform3D::origin(const glm::vec3 &origin)
{
    m_origin = origin;
    m_position = origin + m_translation;
}
void transform3D::rotation(const glm::vec3 &rotation)
{
    m_rotation = rotation;
}

void transform3D::translate(const glm::vec3 &translation)
{
    m_translation += translation;
    m_position += translation;
}
void transform3D::stretch(const glm::vec3 &stretch)
{
    m_scale += stretch;
}
void transform3D::rotate(const glm::vec3 &rotation)
{
    m_rotation += rotation;
}

void transform3D::xposition(const float xp)
{
    m_position.x = xp;
    m_translation.x = xp - m_origin.x;
}
void transform3D::yposition(const float yp)
{
    m_position.y = yp;
    m_translation.y = yp - m_origin.y;
}
void transform3D::zposition(const float zp)
{
    m_position.z = zp;
    m_translation.z = zp - m_origin.z;
}

void transform3D::xtranslation(const float xt)
{
    m_translation.x = xt;
    m_position.x = m_origin.x + xt;
}
void transform3D::ytranslation(const float yt)
{
    m_translation.y = yt;
    m_position.y = m_origin.y + yt;
}
void transform3D::ztranslation(const float zt)
{
    m_translation.z = zt;
    m_position.z = m_origin.z + zt;
}

void transform3D::xtranslate(const float xt)
{
    m_translation.x += xt;
    m_position.x += xt;
}
void transform3D::ytranslate(const float yt)
{
    m_translation.y += yt;
    m_position.y += yt;
}
void transform3D::ztranslate(const float zt)
{
    m_translation.z += zt;
    m_position.z += zt;
}

void transform3D::xscale(const float xs)
{
    m_scale.x = xs;
}
void transform3D::yscale(const float ys)
{
    m_scale.y = ys;
}
void transform3D::zscale(const float zs)
{
    m_scale.z = zs;
}

void transform3D::xstretch(const float xs)
{
    m_scale.x += xs;
}
void transform3D::ystretch(const float ys)
{
    m_scale.y += ys;
}
void transform3D::zstretch(const float zs)
{
    m_scale.z += zs;
}

void transform3D::xorigin(const float xo)
{
    m_origin.x = xo;
    m_position.x = xo + m_translation.x;
}
void transform3D::yorigin(const float yo)
{
    m_origin.y = yo;
    m_position.y = yo + m_translation.y;
}
void transform3D::zorigin(const float zo)
{
    m_origin.z = zo;
    m_position.z = zo + m_translation.z;
}

void transform3D::xrotation(const float xs)
{
    m_rotation.x = xs;
}
void transform3D::yrotation(const float ys)
{
    m_rotation.y = ys;
}
void transform3D::zrotation(const float zs)
{
    m_rotation.z = zs;
}

void transform3D::xrotate(const float xs)
{
    m_rotation.x += xs;
}
void transform3D::yrotate(const float ys)
{
    m_rotation.y += ys;
}
void transform3D::zrotate(const float zs)
{
    m_rotation.z += zs;
}
} // namespace lynx