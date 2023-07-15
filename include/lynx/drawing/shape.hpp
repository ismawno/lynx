#ifndef LYNX_SHAPE_HPP
#define LYNX_SHAPE_HPP

#include "lynx/drawing/drawable.hpp"
#include "lynx/geometry/transform.hpp"
#include "lynx/drawing/model.hpp"

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

    const glm::vec4 &color() const;
    void color(const glm::vec4 &color);

    transform2D transform{};

  protected:
    model2D m_model;

  private:
    topology m_topology;
    virtual void draw(window2D &win) const override;
};

class shape3D : public drawable3D
{
  public:
    template <class... ModelArgs> shape3D(topology tplg, ModelArgs &&...args);

    const glm::vec4 &color() const;
    void color(const glm::vec4 &color);

    transform3D transform{};

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
           const glm::vec4 &color = glm::vec4(1.f));
    rect2D(const glm::vec4 &color);
};
class ellipse2D : public shape2D
{
  public:
    ellipse2D(float ra, float rb, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t partitions = 30);
    ellipse2D(float radius = 1.f, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t partitions = 30);
    ellipse2D(const glm::vec4 &color, std::uint32_t partitions = 30);

    float radius() const;
    void radius(float radius);
};
class polygon2D : public shape2D
{
  public:
    polygon2D(const std::vector<glm::vec2> &local_vertices = {{-1.f, 0.5f}, {1.f, 0.5f}, {0.f, -0.5f}},
              const glm::vec4 &color = glm::vec4(1.f));

    const glm::vec2 &vertex(std::size_t index) const;
    void vertex(std::size_t index, const glm::vec2 &vertex);

    const glm::vec2 &operator[](std::size_t index) const;
    std::size_t size() const;

  private:
    const std::size_t m_size;
};

class rect3D : public shape3D
{
  public:
    rect3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec2 &dimensions = {1.f, 1.f},
           const glm::vec4 &color = glm::vec4(1.f));
    rect3D(const glm::vec4 &color);
};

class ellipse3D : public shape3D
{
  public:
    ellipse3D(float ra, float rb, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t partitions = 30);
    ellipse3D(float radius = 1.f, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t partitions = 30);
    ellipse3D(const glm::vec4 &color, std::uint32_t partitions = 30);

    float radius() const;
    void radius(float radius);
};
class polygon3D : public shape3D
{
    polygon3D(const std::vector<glm::vec3> &local_vertices = {{-1.f, 0.5f, 1.f}, {1.f, 0.5f, 1.f}, {0.f, -0.5f, 1.f}},
              const glm::vec4 &color = glm::vec4(1.f));

    const glm::vec3 &vertex(std::size_t index) const;
    void vertex(std::size_t index, const glm::vec3 &vertex);

    const glm::vec3 &operator[](std::size_t index) const;
    std::size_t size() const;

  private:
    const std::size_t m_size;
};

class ellipsoid3D : public shape3D
{
  public:
    ellipsoid3D(float ra, float rb, float rc, const glm::vec4 &color = glm::vec4(1.f),
                std::uint32_t lat_partitions = 30, std::uint32_t lon_partitions = 15);
    ellipsoid3D(float radius = 1.f, const glm::vec4 &color = glm::vec4(1.f), std::uint32_t lat_partitions = 30,
                std::uint32_t lon_partitions = 15);
    ellipsoid3D(const glm::vec4 &color, std::uint32_t lat_partitions = 30, std::uint32_t lon_partitions = 15);

    float radius() const;
    void radius(float radius);
};
class cube3D : public shape3D
{
  public:
    cube3D(const glm::vec3 &position = glm::vec3(0.f), const glm::vec3 &dimensions = glm::vec3(1.f),
           const glm::vec4 &color = glm::vec4(1.f));
    cube3D(const glm::vec4 &color);
};
} // namespace lynx

#endif