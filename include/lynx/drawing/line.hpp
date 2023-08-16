#ifndef LYNX_LINES_HPP
#define LYNX_LINES_HPP

#include "lynx/drawing/drawable.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/geometry/vertex.hpp"
#include "kit/utility/transform.hpp"
#include "lynx/drawing/color.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
class line2D : public drawable2D
{
  public:
    virtual const glm::vec2 &p1() const = 0;
    virtual const glm::vec2 &p2() const = 0;

    virtual void p1(const glm::vec2 &p1) = 0;
    virtual void p2(const glm::vec2 &p2) = 0;

    virtual const color &color() const = 0;
    virtual void color(const lynx::color &color) = 0;
};

class line3D : public drawable3D
{
  public:
    virtual const glm::vec3 &p1() const = 0;
    virtual const glm::vec3 &p2() const = 0;

    virtual void p1(const glm::vec3 &p1) = 0;
    virtual void p2(const glm::vec3 &p2) = 0;

    virtual const color &color() const = 0;
    virtual void color(const lynx::color &color) = 0;
};

class thin_line2D : public line2D
{
  public:
    thin_line2D(const glm::vec2 &p1 = {0.f, 0.f}, const glm::vec2 &p2 = {1.f, 0.f},
                const lynx::color &color1 = color::white, const lynx::color &color2 = color::white);

    void draw(window2D &win) const override;

    const glm::vec2 &p1() const override;
    const glm::vec2 &p2() const override;

    void p1(const glm::vec2 &p1) override;
    void p2(const glm::vec2 &p2) override;

    const lynx::color &color1() const;
    const lynx::color &color2() const;
    const lynx::color &color() const override;

    void color1(const lynx::color &color1);
    void color2(const lynx::color &color2);
    void color(const lynx::color &color) override;

  private:
    glm::vec2 m_p1;
    glm::vec2 m_p2;

    kit::transform2D m_transform;
    model2D m_model;

    kit::transform2D as_transform() const;
};

class thin_line3D : public line3D
{
  public:
    thin_line3D(const glm::vec3 &p1 = {0.f, 0.f, 0.f}, const glm::vec3 &p2 = {1.f, 0.f, 0.f},
                const lynx::color &color1 = color::white, const lynx::color &color2 = color::white);

    void draw(window3D &win) const override;

    const glm::vec3 &p1() const override;
    const glm::vec3 &p2() const override;

    void p1(const glm::vec3 &p1) override;
    void p2(const glm::vec3 &p2) override;

    const lynx::color &color1() const;
    const lynx::color &color2() const;
    const lynx::color &color() const override;

    void color1(const lynx::color &color1);
    void color2(const lynx::color &color2);
    void color(const lynx::color &color) override;

  private:
    glm::vec3 m_p1;
    glm::vec3 m_p2;

    kit::transform3D m_transform;
    model3D m_model;

    kit::transform3D as_transform() const;
};

class line_strip2D : public drawable2D
{
  public:
    line_strip2D(const std::vector<glm::vec2> &points = {{0.f, 0.f}, {1.f, 0.f}}, const color &color = color::white);
    line_strip2D(const std::vector<vertex2D> &points);

    void draw(window2D &win) const override;

    const vertex2D &operator[](std::size_t index) const;
    const vertex2D &point(std::size_t index) const;

    void point(std::size_t index, const vertex2D &vertex);
    void point(std::size_t index, const glm::vec2 &position);

    void update_points(const std::function<void(vertex2D &)> &for_each_fn);

    const color &color(std::size_t index = 0) const;
    void color(const lynx::color &color);
    void color(std::size_t index, const lynx::color &color);

  private:
    model2D m_model;
};

class line_strip3D : public drawable3D
{
  public:
    line_strip3D(const std::vector<glm::vec3> &points = {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}},
                 const color &color = color::white);
    line_strip3D(const std::vector<vertex3D> &points);

    void draw(window3D &win) const override;

    const vertex3D &operator[](std::size_t index) const;
    const vertex3D &point(std::size_t index) const;

    void point(std::size_t index, const vertex3D &vertex);
    void point(std::size_t index, const glm::vec3 &position);

    void update_points(const std::function<void(vertex3D &)> &for_each_fn);

    const color &color(std::size_t index = 0) const;
    void color(const lynx::color &color);
    void color(std::size_t index, const lynx::color &color);

  private:
    model3D m_model;
};

} // namespace lynx

#endif