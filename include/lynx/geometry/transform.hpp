#ifndef LYNX_TRANSFORM_HPP
#define LYNX_TRANSFORM_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lynx
{
class transform2D
{
  public:
    glm::mat4 transform() const;
    glm::mat4 inverse() const;

    const glm::vec2 &position() const;
    const glm::vec2 &translation() const;
    const glm::vec2 &scale() const;
    const glm::vec2 &origin() const;
    float rotation() const;

    void position(const glm::vec2 &position);
    void translation(const glm::vec2 &translation);
    void scale(const glm::vec2 &scale);
    void origin(const glm::vec2 &origin);
    void rotation(float rotation);

    void translate(const glm::vec2 &translation);
    void stretch(const glm::vec2 &stretch);

    void xposition(float xp);
    void yposition(float yp);

    void xtranslation(float xt);
    void ytranslation(float yt);

    void xtranslate(float xt);
    void ytranslate(float yt);

    void xscale(float xs);
    void yscale(float ys);

    void xstretch(float xs);
    void ystretch(float ys);

    void xorigin(float xo);
    void yorigin(float yo);

    void rotate(float angle);

    static void reset_z_offset_counter();

  private:
    glm::vec2 m_position{0.f};
    glm::vec2 m_translation{0.f};
    glm::vec2 m_scale{1.f};
    glm::vec2 m_origin{0.f};
    float m_rotation = 0.f;

    mutable float m_z_offset = 0.f;
    inline static std::size_t s_z_offset_counter = 0;
};

class transform3D
{
  public:
    glm::mat4 transform() const;
    glm::mat4 inverse() const;

    const glm::vec3 &position() const;
    const glm::vec3 &translation() const;
    const glm::vec3 &scale() const;
    const glm::vec3 &origin() const;
    const glm::vec3 &rotation() const;

    void position(const glm::vec3 &position);
    void translation(const glm::vec3 &translation);
    void scale(const glm::vec3 &scale);
    void origin(const glm::vec3 &origin);
    void rotation(const glm::vec3 &rotation);

    void translate(const glm::vec3 &translation);
    void stretch(const glm::vec3 &stretch);
    void rotate(const glm::vec3 &rotation);

    void xposition(float xp);
    void yposition(float yp);
    void zposition(float zp);

    void xtranslation(float xt);
    void ytranslation(float yt);
    void ztranslation(float zt);

    void xtranslate(float xt);
    void ytranslate(float yt);
    void ztranslate(float zt);

    void xscale(float xs);
    void yscale(float ys);
    void zscale(float zs);

    void xstretch(float xs);
    void ystretch(float ys);
    void zstretch(float zs);

    void xorigin(float xo);
    void yorigin(float yo);
    void zorigin(float zo);

    void xrotation(float xr);
    void yrotation(float yr);
    void zrotation(float zr);

    void xrotate(float xr);
    void yrotate(float yr);
    void zrotate(float zr);

  private:
    glm::vec3 m_position{0.f};
    glm::vec3 m_translation{0.f};
    glm::vec3 m_scale{1.f};
    glm::vec3 m_origin{0.f};
    glm::vec3 m_rotation{0.f};
};
} // namespace lynx

#endif