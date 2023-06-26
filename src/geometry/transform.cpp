#include "lynx/internal/pch.hpp"
#include "lynx/geometry/transform.hpp"

namespace lynx
{
glm::mat4 transform2D::transform() const
{
    m_z_offset = 1.f - ++s_z_offset_counter * std::numeric_limits<float>::epsilon();
    const float c = cosf(m_rotation);
    const float s = sinf(m_rotation);
    return glm::mat4{{
                         c * m_scale.x,
                         s * m_scale.x,
                         0.f,
                         0.f,
                     },
                     {
                         -s * m_scale.y,
                         c * m_scale.y,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                     },
                     {m_translation.x + m_origin.x - c * m_origin.x * m_scale.x + s * m_origin.y * m_scale.y,
                      m_translation.y + m_origin.y - c * m_origin.y * m_scale.y - s * m_origin.x * m_scale.x,
                      m_z_offset, 1.f}};
}
glm::mat4 transform2D::inverse() const
{
    const float c = cosf(m_rotation);
    const float s = sinf(m_rotation);
    return glm::mat4{
        {
            c / m_scale.x,
            -s / m_scale.y,
            0.f,
            0.f,
        },
        {
            s / m_scale.x,
            c / m_scale.y,
            0.f,
            0.f,
        },
        {
            0.f,
            0.f,
            1.f,
            0.f,
        },
        {(m_origin.x * m_scale.x - c * (m_origin.x + m_translation.x) - s * (m_origin.y + m_translation.y)) / m_scale.x,
         (m_origin.y * m_scale.y - c * (m_origin.y + m_translation.y) + s * (m_origin.x + m_translation.x)) / m_scale.y,
         -m_z_offset, 1.f}};
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
    const float c3 = cosf(m_rotation.z);
    const float s3 = sinf(m_rotation.z);
    const float c2 = cosf(m_rotation.x);
    const float s2 = sinf(m_rotation.x);
    const float c1 = cosf(m_rotation.y);
    const float s1 = sinf(m_rotation.y);

    const float e11 = c1 * c3 + s1 * s2 * s3;
    const float e21 = c2 * s3;
    const float e31 = c1 * s2 * s3 - c3 * s1;

    const float e12 = c3 * s1 * s2 - c1 * s3;
    const float e22 = c2 * c3;
    const float e32 = c1 * c3 * s2 + s1 * s3;

    const float e13 = c2 * s1;
    const float e23 = -s2;
    const float e33 = c1 * c2;

    return glm::mat4{{
                         e11 * m_scale.x,
                         e21 * m_scale.x,
                         e31 * m_scale.x,
                         0.f,
                     },
                     {
                         e12 * m_scale.y,
                         e22 * m_scale.y,
                         e32 * m_scale.y,
                         0.f,
                     },
                     {
                         e13 * m_scale.z,
                         e23 * m_scale.z,
                         e33 * m_scale.z,
                         0.f,
                     },
                     {m_translation.x + m_origin.x - e11 * m_origin.x * m_scale.x - e12 * m_origin.y * m_scale.y -
                          e13 * m_origin.z * m_scale.z,
                      m_translation.y + m_origin.y - e22 * m_origin.y * m_scale.y - e21 * m_origin.x * m_scale.x -
                          e23 * m_origin.z * m_scale.z,
                      m_translation.z + m_origin.z - e33 * m_origin.z * m_scale.z - e31 * m_origin.x * m_scale.x -
                          e32 * m_origin.y * m_scale.y,
                      1.f}};
}

glm::mat4 transform3D::inverse() const // YXZ
{
    const float c3 = cosf(m_rotation.z);
    const float s3 = sinf(m_rotation.z);
    const float c2 = cosf(m_rotation.x);
    const float s2 = sinf(m_rotation.x);
    const float c1 = cosf(m_rotation.y);
    const float s1 = sinf(m_rotation.y);

    const float e11 = c1 * c3 + s1 * s2 * s3;
    const float e21 = c2 * s3;
    const float e31 = c1 * s2 * s3 - c3 * s1;

    const float e12 = c3 * s1 * s2 - c1 * s3;
    const float e22 = c2 * c3;
    const float e32 = c1 * c3 * s2 + s1 * s3;

    const float e13 = c2 * s1;
    const float e23 = -s2;
    const float e33 = c1 * c2;

    return glm::mat4{{
                         e11 / m_scale.x,
                         e12 / m_scale.y,
                         e13 / m_scale.z,
                         0.f,
                     },
                     {
                         e21 / m_scale.x,
                         e22 / m_scale.y,
                         e23 / m_scale.z,
                         0.f,
                     },
                     {
                         e31 / m_scale.x,
                         e32 / m_scale.y,
                         e33 / m_scale.z,
                         0.f,
                     },
                     {(m_origin.x * m_scale.x - e11 * (m_origin.x + m_translation.x) -
                       e21 * (m_origin.y + m_translation.y) - e31 * (m_origin.z + m_translation.z)) /
                          m_scale.x,
                      (m_origin.y * m_scale.y - e22 * (m_origin.y + m_translation.y) -
                       e12 * (m_origin.x + m_translation.x) - e32 * (m_origin.z + m_translation.z)) /
                          m_scale.y,
                      (m_origin.z * m_scale.z - e33 * (m_origin.z + m_translation.z) -
                       e13 * (m_origin.x + m_translation.x) - e23 * (m_origin.y + m_translation.y)) /
                          m_scale.z,
                      1.f}};
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