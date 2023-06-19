#include "lynx/pch.hpp"
#include "lynx/transform.hpp"

namespace lynx
{
transform2D::operator glm::mat4() const
{
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
                     {position.x, position.y, 0.5f, 1.0f}};
}

transform3D::operator glm::mat4() const // YXZ
{
    const float c3 = cosf(rotation.z);
    const float s3 = sinf(rotation.z);
    const float c2 = cosf(rotation.x);
    const float s2 = sinf(rotation.x);
    const float c1 = cosf(rotation.y);
    const float s1 = sinf(rotation.y);
    return glm::mat4{{
                         scale.x * (c1 * c3 + s1 * s2 * s3),
                         scale.x * (c2 * s3),
                         scale.x * (c1 * s2 * s3 - c3 * s1),
                         0.f,
                     },
                     {
                         scale.y * (c3 * s1 * s2 - c1 * s3),
                         scale.y * (c2 * c3),
                         scale.y * (c1 * c3 * s2 + s1 * s3),
                         0.f,
                     },
                     {
                         scale.z * (c2 * s1),
                         scale.z * (-s2),
                         scale.z * (c1 * c2),
                         0.f,
                     },
                     {position.x, position.y, position.z, 1.0f}};
}
} // namespace lynx