#include "lynx/internal/pch.hpp"
#include "lynx/drawing/color.hpp"

namespace lynx
{
const color color::red{255u, 0u, 0u};
const color color::green{0u, 255u, 0u};
const color color::blue{0u, 0u, 255u};
const color color::magenta{255u, 0u, 255u};
const color color::cyan{0u, 255u, 255u};
const color color::orange{255u, 165u, 0u};
const color color::yellow{255u, 255u, 0u};
const color color::black{0u};
const color color::pink{255u, 192u, 203u};
const color color::purple{191u, 64u, 191u};
const color color::white{255u};

static constexpr float one_over_255 = 1.f / 255.f;
static std::uint32_t to_int(const float val)
{
    return (std::uint32_t)(val * 255.f);
}

static float to_float(const std::uint32_t val)
{
    return val * one_over_255;
}

color::color(const float val) : normalized(val)
{
}
color::color(const std::uint32_t val) : normalized(val / 255.f)
{
}

color::color(const glm::vec4 &rgba) : normalized(rgba)
{
}
color::color(const glm::vec3 &rgb) : normalized(rgb, 1.f)
{
}

color::color(const float r, const float g, const float b, const float a) : normalized(r, g, b, a)
{
}
color::color(const std::uint32_t r, const std::uint32_t g, const std::uint32_t b, const std::uint32_t a)
    : normalized(to_float(r), to_float(g), to_float(b), to_float(a))
{
}

std::uint32_t color::r() const
{
    return to_int(normalized.r);
}
std::uint32_t color::g() const
{
    return to_int(normalized.g);
}
std::uint32_t color::b() const
{
    return to_int(normalized.b);
}
std::uint32_t color::a() const
{
    return to_int(normalized.a);
}

void color::r(std::uint32_t r)
{
    normalized.r = to_float(r);
}
void color::g(std::uint32_t g)
{
    normalized.g = to_float(g);
}
void color::b(std::uint32_t b)
{
    normalized.b = to_float(b);
}
void color::a(std::uint32_t a)
{
    normalized.a = to_float(a);
}

const float *color::ptr() const
{
    return glm::value_ptr(normalized);
}
float *color::ptr()
{
    return glm::value_ptr(normalized);
}
} // namespace lynx