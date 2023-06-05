#ifndef LYNX_MODEL_HPP
#define LYNX_MODEL_HPP

#include "lynx/core.hpp"
#include "lynx/vertex_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lynx
{
struct vertex2D
{
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};

struct transform2D
{
    glm::vec2 translation{0.f};
    glm::vec2 scale{1.f};
    float rotation = 0.f;
    operator glm::mat4() const;
};

struct vertex3D
{
    glm::vec3 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};

struct transform3D
{
    glm::vec3 translation{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 rotation{0.f};
    operator glm::mat4() const;
};

class model
{
  public:
    model(const device &dev, const std::vector<vertex2D> &vertices);
    model(const device &dev, const std::vector<vertex3D> &vertices);

    void bind(VkCommandBuffer command_buffer) const;
    void draw(VkCommandBuffer command_buffer) const;

  private:
    vertex_buffer m_vertex_buffer;
    std::size_t m_vertex_count;
};

class model2D : public model
{
  public:
    model2D(const device &dev, const std::vector<vertex2D> &vertices);

    static std::vector<vertex2D> triangle(const glm::vec3 &color = glm::vec3(1.f));

    transform2D transform;
};

class model3D : public model
{
  public:
    model3D(const device &dev, const std::vector<vertex3D> &vertices);

    static std::vector<vertex3D> cube(const glm::vec3 &color = glm::vec3(1.f));

    transform3D transform;
};
} // namespace lynx

#endif