#ifndef LYNX_SHAPE_HPP
#define LYNX_SHAPE_HPP

#include "lynx/drawing/drawable.hpp"
#include "kit/utility/transform.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/drawing/color.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lynx
{
struct vertex2D;
struct vertex3D;

class shape2D : public drawable2D
{
  public:
    template <class... ModelArgs> shape2D(topology tplg, ModelArgs &&...args);

    const lynx::color &color() const;
    void color(const lynx::color &color);

    const lynx::color &outline_color() const;
    void outline_color(const lynx::color &color);

    float outline_thickness() const;
    void outline_thickness(float thickness);

    kit::transform2D transform{};

  protected:
    model2D m_model;

  private:
    mutable model2D m_outline_model;
    float m_outline_thickness = 0.f;

    topology m_topology;

    void update_outline_thickness(); // Always update vertices
    void draw(window2D &win) const override;
};

class shape3D : public drawable3D
{
  public:
    template <class... ModelArgs> shape3D(topology tplg, ModelArgs &&...args);

    const lynx::color &color() const;
    void color(const lynx::color &color);

    kit::transform3D transform{};

  protected:
    model3D m_model;

  private:
    topology m_topology;
    virtual void draw(window3D &win) const override;
};

class rect2D : public shape2D
{
  public:
    rect2D(const glm::vec2 &position = {0.f, 0.f}, const glm::vec2 &dimensions = {1.f, 1.f},
           const lynx::color &color = lynx::color::white);
    rect2D(const lynx::color &color);
};
class ellipse2D : public shape2D
{
  public:
    ellipse2D(float ra, float rb, const lynx::color &color = lynx::color::white, std::uint32_t partitions = 30);
    ellipse2D(float radius = 1.f, const lynx::color &color = lynx::color::white, std::uint32_t partitions = 30);
    ellipse2D(const lynx::color &color, std::uint32_t partitions = 30);

    float radius() const;
    void radius(float radius);
};
class polygon2D : public shape2D
{
  public:
    polygon2D(const std::vector<glm::vec2> &local_vertices = {{-1.f, 0.5f}, {1.f, 0.5f}, {0.f, -0.5f}},
              const lynx::color &color = lynx::color::white);
    polygon2D(const std::vector<vertex2D> &local_vertices, const lynx::color &center_color = lynx::color::white);

    const vertex2D &operator[](std::size_t index) const;
    const vertex2D &vertex(std::size_t index) const;

    void vertex(std::size_t index, const vertex2D &vertex);
    void vertex(std::size_t index, const glm::vec2 &position);

    void update_vertices(const std::function<void(std::size_t, vertex2D &)> &for_each_fn);
    std::size_t size() const;

    const lynx::color &color(std::size_t index = 0) const; // APLICAR A POLYGON3D
    void color(const lynx::color &color);                  // Getter y setter para el center color
    void color(std::size_t index, const lynx::color &color);

  private:
    std::size_t m_size;
};

class rect3D : public shape3D
{
  public:
    rect3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec2 &dimensions = {1.f, 1.f},
           const lynx::color &color = lynx::color::white);
    rect3D(const lynx::color &color);
};

class ellipse3D : public shape3D
{
  public:
    ellipse3D(float ra, float rb, const lynx::color &color = lynx::color::white, std::uint32_t partitions = 30);
    ellipse3D(float radius = 1.f, const lynx::color &color = lynx::color::white, std::uint32_t partitions = 30);
    ellipse3D(const lynx::color &color, std::uint32_t partitions = 30);

    float radius() const;
    void radius(float radius);
};
class polygon3D : public shape3D
{
    polygon3D(const std::vector<glm::vec3> &local_vertices = {{-1.f, 0.5f, 1.f}, {1.f, 0.5f, 1.f}, {0.f, -0.5f, 1.f}},
              const lynx::color &color = lynx::color::white);
    polygon3D(const std::vector<vertex3D> &local_vertices, const lynx::color &center_color = lynx::color::white);

    const vertex3D &operator[](std::size_t index) const;
    const vertex3D &vertex(std::size_t index) const;

    void vertex(std::size_t index, const vertex3D &vertex);
    void vertex(std::size_t index, const glm::vec3 &position);

    void update_vertices(const std::function<void(std::size_t, vertex3D &)> &for_each_fn);
    std::size_t size() const;

    const lynx::color &color(std::size_t index = 0) const; // APLICAR A POLYGON3D
    void color(const lynx::color &color);                  // Getter y setter para el center color
    void color(std::size_t index, const lynx::color &color);

  private:
    std::size_t m_size;
};

class ellipsoid3D : public shape3D
{
  public:
    ellipsoid3D(float ra, float rb, float rc, const lynx::color &color = lynx::color::white,
                std::uint32_t lat_partitions = 30, std::uint32_t lon_partitions = 15);
    ellipsoid3D(float radius = 1.f, const lynx::color &color = lynx::color::white, std::uint32_t lat_partitions = 30,
                std::uint32_t lon_partitions = 15);
    ellipsoid3D(const lynx::color &color, std::uint32_t lat_partitions = 30, std::uint32_t lon_partitions = 15);

    float radius() const;
    void radius(float radius);
};
class cube3D : public shape3D
{
  public:
    cube3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec3 &dimensions = glm::vec3(1.f),
           const lynx::color &color = lynx::color::white);
    cube3D(const lynx::color &color);
};
} // namespace lynx

#endif