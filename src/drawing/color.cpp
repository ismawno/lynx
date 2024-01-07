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
const color color::transparent{white, 0u};

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
    KIT_ASSERT_ERROR(val <= 1.f, "Color floating values must be in the range [0, 1]");
}
color::color(const std::uint32_t val) : normalized(val / 255.f)
{
    KIT_ASSERT_ERROR(val < 256, "Color integer values must be in the range [0, 255]");
}

color::color(const glm::vec4 &rgba) : normalized(rgba)
{
    KIT_ASSERT_ERROR(rgba.r <= 1.f, "Red value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgba.g <= 1.f, "Green value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgba.b <= 1.f, "Blue value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgba.a <= 1.f, "Alpha value must be in the range [0, 1]");
}
color::color(const glm::vec3 &rgb) : normalized(rgb, 1.f)
{
    KIT_ASSERT_ERROR(rgb.r <= 1.f, "Red value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgb.g <= 1.f, "Green value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgb.b <= 1.f, "Blue value must be in the range [0, 1]");
}

color::color(const float r, const float g, const float b, const float a) : normalized(r, g, b, a)
{
    KIT_ASSERT_ERROR(r <= 1.f, "Red value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(g <= 1.f, "Green value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(b <= 1.f, "Blue value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(a <= 1.f, "Alpha value must be in the range [0, 1]");
}
color::color(const std::uint32_t r, const std::uint32_t g, const std::uint32_t b, const std::uint32_t a)
    : normalized(to_float(r), to_float(g), to_float(b), to_float(a))
{
    KIT_ASSERT_ERROR(r < 256, "Red value must be in the range [0, 255]");
    KIT_ASSERT_ERROR(g < 256, "Green value must be in the range [0, 255]");
    KIT_ASSERT_ERROR(b < 256, "Blue value must be in the range [0, 255]");
    KIT_ASSERT_ERROR(a < 256, "Alpha value must be in the range [0, 255]");
}

color::color(const color &rgb, float a) : normalized(glm::vec3(rgb.normalized), a)
{
    KIT_ASSERT_ERROR(a <= 1.f, "Alpha value must be in the range [0, 1]");
}
color::color(const color &rgb, std::uint32_t a) : normalized(glm::vec3(rgb.normalized), to_float(a))
{
    KIT_ASSERT_ERROR(a < 256, "Alpha value must be in the range [0, 255]");
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
    KIT_ASSERT_ERROR(r < 256, "Color integer values must be in the range [0, 255]")
    normalized.r = to_float(r);
}
void color::g(std::uint32_t g)
{
    KIT_ASSERT_ERROR(g < 256, "Color integer values must be in the range [0, 255]")
    normalized.g = to_float(g);
}
void color::b(std::uint32_t b)
{
    KIT_ASSERT_ERROR(b < 256, "Color integer values must be in the range [0, 255]")
    normalized.b = to_float(b);
}
void color::a(std::uint32_t a)
{
    KIT_ASSERT_ERROR(a < 256, "Color integer values must be in the range [0, 255]")
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

static float lerp(const float a, const float b, const float t)
{
    return a + (b - a) * t;
}

color color::lerp(const color &c1, const color &c2, float t)
{
    KIT_ASSERT_ERROR(t >= 0.f && t <= 1.f, "t must lie in [0, 1]")
    const float r = ::lynx::lerp(c1.normalized.r, c2.normalized.r, t);
    const float g = ::lynx::lerp(c1.normalized.g, c2.normalized.g, t);
    const float b = ::lynx::lerp(c1.normalized.b, c2.normalized.b, t);
    const float a = ::lynx::lerp(c1.normalized.a, c2.normalized.a, t);
    return color{r, g, b, a};
}
} // namespace lynx