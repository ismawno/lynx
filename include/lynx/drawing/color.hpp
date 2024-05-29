#pragma once

#include "kit/utility/type_constraints.hpp"

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
    color(const glm::vec3 &rgb, float a = 1.f);

    color(float r, float g, float b, float a = 1.f);
    color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a = 255);

    color(const color &rgb, float a);
    color(const color &rgb, std::uint32_t a);

    union {
        glm::vec4 rgba;
        glm::vec3 rgb;
    };

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

    operator const glm::vec4 &() const;
    operator const glm::vec3 &() const;

    color &operator+=(const color &rhs);
    color &operator-=(const color &rhs);
    color &operator*=(const color &rhs);
    color &operator/=(const color &rhs);
    template <typename T> color &operator*=(const T &rhs)
    {
        rgb = glm::clamp(rgb * rhs, 0.f, 1.f);
        return *this;
    }
    template <typename T> color &operator/=(const T &rhs)
    {
        rgb = glm::clamp(rgb / rhs, 0.f, 1.f);
        return *this;
    }

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

color operator+(const color &lhs, const color &rhs);
color operator-(const color &lhs, const color &rhs);
color operator*(const color &lhs, const color &rhs);
color operator/(const color &lhs, const color &rhs);
template <typename T> color operator*(const color &lhs, const T &rhs)
{
    return color{glm::clamp(lhs.rgb * rhs, 0.f, 1.f)};
}
template <typename T> color operator*(const T &lhs, const color &rhs)
{
    return color{glm::clamp(lhs * rhs.rgb, 0.f, 1.f)};
}
template <typename T> color operator/(const color &lhs, const T &rhs)
{
    return color{glm::clamp(lhs.rgb / rhs, 0.f, 1.f)};
}
template <typename T> color operator/(const T &lhs, const color &rhs)
{
    return color{glm::clamp(lhs / rhs.rgb, 0.f, 1.f)};
}

bool operator==(const color &lhs, const color &rhs);
bool operator!=(const color &lhs, const color &rhs);

template <std::size_t N>
    requires(N > 1)
class gradient
{
  public:
    template <class... ArrayArgs>
        requires kit::NoCopyCtorOverride<gradient, ArrayArgs...>
    gradient(ArrayArgs &&...args) : m_colors({std::forward<ArrayArgs>(args)...})
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
        return color{m_colors[idx].rgba + (m_colors[idx + 1].rgba - m_colors[idx].rgba) * t};
    }

    static float map(const float x, const float mm1, const float mx1, const float mm2, const float mx2)
    {
        return mm2 + (x - mm1) * (mx2 - mm2) / (mx1 - mm1);
    }
};
} // namespace lynx
