#ifndef LYNX_LINES_HPP
#define LYNX_LINES_HPP

#include "lynx/drawable.hpp"
#include "lynx/vertex.hpp"
#include "lynx/transform.hpp"
#include "lynx/core.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
class model2D;
class model3D;

class thin_line2D : public drawable2D
{
  public:
    thin_line2D(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec4 &color1 = glm::vec4(1.f),
                const glm::vec4 &color2 = glm::vec4(1.f));

    void draw(window2D &win) const override;

    const glm::vec2 &p1() const;
    const glm::vec2 &p2() const;

    void p1(const glm::vec2 &p1);
    void p2(const glm::vec2 &p2);

    const glm::vec4 &color1() const;
    const glm::vec4 &color2() const;

    void color1(const glm::vec4 &color1);
    void color2(const glm::vec4 &color2);

  private:
    glm::vec2 m_p1;
    glm::vec2 m_p2;

    transform2D m_transform;
    ref<model2D> m_model;

    transform2D as_transform() const;
};

class thin_line3D : public drawable3D
{
  public:
    thin_line3D(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec4 &color1 = glm::vec4(1.f),
                const glm::vec4 &color2 = glm::vec4(1.f));

    void draw(window3D &win) const override;

    const glm::vec3 &p1() const;
    const glm::vec3 &p2() const;

    void p1(const glm::vec3 &p1);
    void p2(const glm::vec3 &p2);

    const glm::vec4 &color1() const;
    const glm::vec4 &color2() const;

    void color1(const glm::vec4 &color1);
    void color2(const glm::vec4 &color2);

  private:
    glm::vec3 m_p1;
    glm::vec3 m_p2;

    transform3D m_transform;
    ref<model3D> m_model;

    transform3D as_transform() const;
};
} // namespace lynx

#endif