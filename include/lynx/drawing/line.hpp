#ifndef LYNX_LINES_HPP
#define LYNX_LINES_HPP

#include "lynx/drawing/drawable.hpp"
#include "lynx/geometry/vertex.hpp"
#include "lynx/geometry/transform.hpp"
#include "lynx/internal/core.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
class model2D;
class model3D;

class line2D : public drawable2D
{
  public:
    line2D(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec4 &color1 = glm::vec4(1.f),
           const glm::vec4 &color2 = glm::vec4(1.f));

    line2D(const line2D &);
    line2D &operator=(const line2D &);

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

class line3D : public drawable3D
{
  public:
    line3D(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec4 &color1 = glm::vec4(1.f),
           const glm::vec4 &color2 = glm::vec4(1.f));

    line3D(const line3D &);
    line3D &operator=(const line3D &);

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

class line_strip2D : public drawable2D
{
  public:
    line_strip2D(const std::vector<glm::vec2> &points, const glm::vec4 &color = glm::vec4(1.f));
    line_strip2D(const std::vector<vertex2D> &points);

    void draw(window2D &win) const override;

    const vertex2D &operator[](std::size_t index) const;
    const vertex2D &point(std::size_t index) const;
    void point(std::size_t index, const vertex2D &vertex);
};

class line_strip3D : public drawable3D
{
  public:
    line_strip3D(const std::vector<glm::vec3> &points, const glm::vec4 &color = glm::vec4(1.f));
    line_strip3D(const std::vector<vertex3D> &points);

    void draw(window3D &win) const override;

    const vertex3D &operator[](std::size_t index) const;
    const vertex3D &point(std::size_t index) const;
    void point(std::size_t index, const vertex3D &vertex);
};

} // namespace lynx

#endif