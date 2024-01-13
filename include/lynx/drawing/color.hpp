#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <cstdint>
#include <array>

namespace lynx
{
struct color
{
    color(float val = 1.f);
    color(std::uint32_t val);

    color(const glm::vec4 &rgba);
    color(const glm::vec3 &rgb);

    color(float r, float g, float b, float a = 1.f);
    color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a = 255);

    color(const color &rgb, float a);
    color(const color &rgb, std::uint32_t a);

    glm::vec4 normalized;

    std::uint32_t r() const;
    std::uint32_t g() const;
    std::uint32_t b() const;
    std::uint32_t a() const;

    void r(std::uint32_t r);
    void g(std::uint32_t g);
    void b(std::uint32_t b);
    void a(std::uint32_t a);

    const float *ptr() const;
    float *ptr();

    static const color red;
    static const color green;
    static const color blue;
    static const color magenta;
    static const color cyan;
    static const color orange;
    static const color yellow;
    static const color black;
    static const color pink;
    static const color purple;
    static const color white;
    static const color transparent;
};

template <std::size_t N>
    requires(N != 0)
class gradient
{
  public:
    template <class... Args> gradient(Args &&...args) : m_colors({std::forward<Args>(args)...})
    {
    }

    color evaluate(const float t) const
    {
        KIT_ASSERT_ERROR(t >= 0.f && t <= 1.f, "t must lie in [0, 1]")
        const float dx = 1.f / (float)(N - 1);
        for (std::size_t i = 0; i < N - 1; i++)
        {
            const float mm = i * dx;
            const float mx = (i + 1) * dx;
            if (t >= mm && t <= mx)
            {
                const float mt = map(t, mm, mx, 0.f, 1.f);
                return lerp(mt, i);
            }
        }
        return color{};
    }

  private:
    std::array<color, N> m_colors;

    color lerp(const float t, const std::size_t idx) const
    {
        return color{m_colors[idx].normalized + (m_colors[idx + 1].normalized - m_colors[idx].normalized) * t};
    }

    static float map(const float x, const float mm1, const float mx1, const float mm2, const float mx2)
    {
        return mm2 + (x - mm1) * (mx2 - mm2) / (mx1 - mm1);
    }
};
} // namespace lynx
