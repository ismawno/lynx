#ifndef LYNX_COLOR_HPP
#define LYNX_COLOR_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <cstdint>

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

    glm::vec4 normalized;

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
};
} // namespace lynx

#endif