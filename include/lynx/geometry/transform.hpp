#ifndef LYNX_TRANSFORM_HPP
#define LYNX_TRANSFORM_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lynx
{
struct transform2D
{
  public:
    glm::vec2 position{0.f};
    glm::vec2 scale{1.f};
    glm::vec2 origin{0.f};
    float rotation = 0.f;

    struct trigonometry
    {
        float c;
        float s;
    };
    struct rbasis
    {
        glm::vec2 x;
        glm::vec2 y;
    };

    glm::mat4 transform() const;
    glm::mat4 inverse() const;

    glm::mat4 transform_as_camera() const;
    glm::mat4 inverse_as_camera() const;

    void translate_local(const glm::vec2 &dpos);
    void xtranslate_local(float dx);
    void ytranslate_local(float dy);

    static trigonometry trigonometric_functions(float rotation);
};

struct transform3D
{
  public:
    glm::vec3 position{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 origin{0.f};
    glm::vec3 rotation{0.f};

    struct trigonometry
    {
        float c1;
        float s1;
        float c2;
        float s2;
        float c3;
        float s3;
    };
    struct rbasis
    {
        glm::vec3 x;
        glm::vec3 y;
        glm::vec3 z;
    };

    glm::mat4 transform() const;
    glm::mat4 inverse() const;

    glm::mat4 transform_as_camera() const;
    glm::mat4 inverse_as_camera() const;

    void translate_local(const glm::vec3 &dpos);
    void xtranslate_local(float dx);
    void ytranslate_local(float dy);
    void ztranslate_local(float dz);

    static trigonometry trigonometric_functions(const glm::vec3 &rotation);
    static rbasis rotation_basis(const glm::vec3 &rotation);
    static rbasis inverse_rotation_basis(const glm::vec3 &rotation);
};
} // namespace lynx

#endif