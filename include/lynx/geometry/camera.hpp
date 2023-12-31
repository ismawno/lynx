#pragma once

#include "kit/utility/transform.hpp"
#include "lynx/internal/dimension.hpp"

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#undef near
#undef far
#endif

namespace lynx
{
template <typename Dim> class camera
{
  public:
    using vec_t = typename Dim::vec_t;
    using transform_t = typename Dim::transform_t;

    virtual ~camera() = default;
    virtual void update_transformation_matrices() = 0;

    transform_t transform;

    virtual void keep_aspect_ratio(float aspect);
    vec_t screen_to_world(const glm::vec2 &screen_pos) const;
    glm::vec2 world_to_screen(const vec_t &world_pos) const;

    const glm::mat4 &projection() const;
    const glm::mat4 &inverse_projection() const;

    void flip_y_axis();

  protected:
    glm::mat4 m_projection{1.f};
    glm::mat4 m_inv_projection{1.f};
    bool m_y_flipped = false;
};

using camera2D = camera<dimension::two>;
class camera3D : public camera<dimension::three>
{
  public:
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

    float size() const;
    void size(float size);

    void update_transformation_matrices() override;
};

class orthographic3D : public camera3D
{
  public:
    orthographic3D() = default;
    orthographic3D(float aspect, float xy_size, float depth, const glm::mat3 &rotation = glm::mat3(1.f));
    orthographic3D(const glm::vec3 &position, float aspect, float xy_size, float depth,
                   const glm::mat3 &rotation = glm::mat3(1.f));

    orthographic3D(const glm::vec3 &size, const glm::mat3 &rotation = glm::mat3(1.f));
    orthographic3D(const glm::vec3 &position, const glm::vec3 &size, const glm::mat3 &rotation = glm::mat3(1.f));

    float size() const;
    void size(float size);

    void update_transformation_matrices() override;
};

class perspective3D : public camera3D
{
  public:
    perspective3D() = default;
    perspective3D(float aspect, float fovy, const glm::mat3 &rotation = glm::mat3(1.f), float near = 0.1f,
                  float far = 10.f);
    perspective3D(const glm::vec3 &position, float aspect, float fovy, const glm::mat3 &rotation = glm::mat3(1.f),
                  float near = 0.1f, float far = 10.f);

    float near;
    float far;
    float fov;

    void keep_aspect_ratio(float aspect) override;
    void update_transformation_matrices() override;

  private:
    float m_aspect;
};

} // namespace lynx
