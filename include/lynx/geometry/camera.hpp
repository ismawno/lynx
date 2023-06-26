#ifndef LYNX_CAMERA_HPP
#define LYNX_CAMERA_HPP

#include "lynx/geometry/transform.hpp"

namespace lynx
{
class camera
{
  public:
    virtual ~camera() = default;
    virtual void update_transformation_matrices() = 0;
    virtual void keep_aspect_ratio(float aspect) = 0;

    const glm::mat4 &projection() const;
    const glm::mat4 &view() const;

    const glm::mat4 &inverse_projection() const;
    const glm::mat4 &inverse_view() const;

  protected:
    glm::mat4 m_projection{1.f};
    glm::mat4 m_view{1.f};

    glm::mat4 m_inv_projection{1.f};
    glm::mat4 m_inv_view{1.f};

  private:
    camera &operator=(const camera &) = delete;
};

class camera2D : public camera
{
  public:
    transform2D transform{};

    void keep_aspect_ratio(float aspect) override;
    glm::vec2 screen_to_world(const glm::vec2 &screen_pos) const;
    glm::vec2 world_to_screen(const glm::vec2 &world_pos) const;
};

class camera3D : public camera
{
  public:
    transform3D transform{};

    void keep_aspect_ratio(float aspect) override;

    glm::vec3 screen_to_world(const glm::vec2 &screen_pos, float z_screen = 0.5f) const;
    glm::vec2 world_to_screen(const glm::vec3 &world_pos) const;

    void point_towards(const glm::vec3 &direction);
    void point_to(const glm::vec3 &position);
};

class orthographic2D : public camera2D
{
  public:
    orthographic2D() = default;
    orthographic2D(float aspect, float size, float rotation = 0.f);
    orthographic2D(const glm::vec2 &position, float aspect, float size, float rotation = 0.f);
    orthographic2D(const glm::vec2 &size, float rotation = 0.f);
    orthographic2D(const glm::vec2 &position, const glm::vec2 &size, float rotation = 0.f);

    void update_transformation_matrices() override;

  private:
    void update_projection();
    void update_view();
};

class orthographic3D : public camera3D
{
  public:
    orthographic3D() = default;
    orthographic3D(float aspect, float xy_size, float depth, const glm::vec3 &rotation = glm::vec3(0.f));
    orthographic3D(const glm::vec3 &position, float aspect, float xy_size, float depth,
                   const glm::vec3 &rotation = glm::vec3(0.f));

    orthographic3D(const glm::vec3 &size, const glm::vec3 &rotation = glm::vec3(0.f));
    orthographic3D(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &rotation = glm::vec3(0.f));

    void update_transformation_matrices() override;

  private:
    void update_projection();
    void update_view();
};

class perspective3D : public camera3D
{
  public:
    perspective3D() = default;
    perspective3D(float aspect, float fovy, const glm::vec3 &rotation = glm::vec3(0.f), float near = 0.1f,
                  float far = 10.f);
    perspective3D(const glm::vec3 &position, float aspect, float fovy, const glm::vec3 &rotation = glm::vec3(0.f),
                  float near = 0.1f, float far = 10.f);

    float near() const;
    float far() const;
    float fov() const;

    void near(float near);
    void far(float far);
    void fov(float aspect, float fovy);

    void update_transformation_matrices() override;

  private:
    float m_near;
    float m_far;

    void update_projection();
    void update_view();
};

} // namespace lynx

#endif