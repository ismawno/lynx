#include "lynx/internal/pch.hpp"
#include "lynx/geometry/transform.hpp"

namespace lynx
{
void transform2D::reset_z_offset_counter()
{
    s_z_offset_counter = 0;
}
transform2D::operator glm::mat4() const
{
    const float z_offset = 1.f - ++s_z_offset_counter * std::numeric_limits<float>::epsilon();
    const float c = cosf(rotation);
    const float s = sinf(rotation);
    return glm::mat4{{
                         scale.x * c,
                         scale.x * s,
                         0.f,
                         0.f,
                     },
                     {
                         -scale.y * s,
                         scale.y * c,
                         0.f,
                         0.f,
                     },
                     {
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                     },
                     {position.x + origin.x * (1.f - c) + origin.y * s,
                      position.y + origin.y * (1.f - c) - origin.x * s, z_offset, 1.f}};
}

transform3D::operator glm::mat4() const // YXZ
{
    const float c3 = cosf(rotation.z);
    const float s3 = sinf(rotation.z);
    const float c2 = cosf(rotation.x);
    const float s2 = sinf(rotation.x);
    const float c1 = cosf(rotation.y);
    const float s1 = sinf(rotation.y);

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
                         scale.x * e11,
                         scale.x * e21,
                         scale.x * e31,
                         0.f,
                     },
                     {
                         scale.y * e12,
                         scale.y * e22,
                         scale.y * e32,
                         0.f,
                     },
                     {
                         scale.z * e13,
                         scale.z * e23,
                         scale.z * e33,
                         0.f,
                     },
                     {position.x + origin.x * (1.f - e11) - e12 * origin.y - e13 * origin.z,
                      position.y + origin.y * (1.f - e22) - e21 * origin.x - e23 * origin.z,
                      position.z + origin.z * (1.f - e33) - e31 * origin.x - e32 * origin.y, 1.f}};
}
} // namespace lynx