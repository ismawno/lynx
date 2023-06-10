#ifndef LYNX_CAMERA_HPP
#define LYNX_CAMERA_HPP

#include "lynx/transform.hpp"

namespace lynx
{
class camera
{
  public:
    virtual ~camera() = default;
    virtual void update_transformation_matrices() = 0;

    const glm::mat4 &projection() const;
    const glm::mat4 &view() const;

  protected:
    glm::mat4 m_projection{1.f};
    glm::mat4 m_view{1.f};
};

class camera2D : public camera
{
  public:
    transform2D transform;
};

class camera3D : public camera
{
  public:
    transform3D transform;
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

// class camera3D
// {
//   public:
//     camera3D() = default;
//     camera3D(const glm::vec3 &orthographic_dimensions);
//     camera3D(const glm::vec3 &min, const glm::vec3 &max);
//     camera3D(float fovy, float aspect, float near, float far);

//     // void ortho
//     void orthographic_projection(const glm::vec3 &dimensions);
//     void orthographic_projection(const glm::vec3 &min, const glm::vec3 &max);
//     void perspective_projection(float fovy, float aspect, float near, float far);

//     void view_as_direction(const glm::vec3 &position, const glm::vec3 &direction,
//                            const glm::vec3 &up = {0.f, -1.f, 0.f});
//     void view_as_target(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up = {0.f, -1.f, 0.f});
//     void view_as_transform(const glm::vec3 &position, const glm::vec3 &rotation);

//     const glm::mat4 &projection_matrix() const;
//     const glm::mat4 &view_matrix() const;

//   private:
//     glm::mat4 m_projection{1.f};
//     glm::mat4 m_view{1.f};
// };
} // namespace lynx

#endif