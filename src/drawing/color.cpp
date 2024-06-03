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

const std::unordered_map<std::string, color> color::map{
    {"red", red},   {"green", green},   {"blue", blue},     {"magenta", magenta},
    {"cyan", cyan}, {"orange", orange}, {"yellow", yellow}, {"black", black},
    {"pink", pink}, {"purple", purple}, {"white", white},   {"transparent", transparent}};

static constexpr float one_over_255 = 1.f / 255.f;
static std::uint8_t to_int(const float val)
{
    return (std::uint8_t)(val * 255.f);
}

static float to_float(const std::uint8_t val)
{
    return val * one_over_255;
}
static float to_float(const std::uint32_t val)
{
    return val * one_over_255;
}

color::color(const float val) : color(val, val, val, 1.f)
{
    KIT_ASSERT_ERROR(val <= 1.f && val >= 0.f, "Color floating values must be in the range [0, 1]");
}
color::color(const std::uint32_t val) : color(val, val, val, 255u)
{
}

color::color(const glm::vec4 &rgba) : rgba(rgba)
{
    KIT_ASSERT_ERROR(rgba.r <= 1.f && rgba.r >= 0.f, "Red value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgba.g <= 1.f && rgba.g >= 0.f, "Green value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgba.b <= 1.f && rgba.b >= 0.f, "Blue value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgba.a <= 1.f && rgba.a >= 0.f, "Alpha value must be in the range [0, 1]");
}
color::color(const glm::vec3 &rgb, float a) : rgba(rgb, a)
{
    KIT_ASSERT_ERROR(rgb.r <= 1.f && rgb.r >= 0.f, "Red value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgb.g <= 1.f && rgb.g >= 0.f, "Green value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(rgb.b <= 1.f && rgb.b >= 0.f, "Blue value must be in the range [0, 1]");
}

color::color(const float r, const float g, const float b, const float a) : rgba(r, g, b, a)
{
    KIT_ASSERT_ERROR(r <= 1.f && r >= 0.f, "Red value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(g <= 1.f && g >= 0.f, "Green value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(b <= 1.f && b >= 0.f, "Blue value must be in the range [0, 1]");
    KIT_ASSERT_ERROR(a <= 1.f && a >= 0.f, "Alpha value must be in the range [0, 1]");
}
color::color(const std::uint32_t r, const std::uint32_t g, const std::uint32_t b, const std::uint32_t a)
    : rgba(to_float(r), to_float(g), to_float(b), to_float(a))
{
    KIT_ASSERT_ERROR(r < 256, "Red value must be in the range [0, 255]");
    KIT_ASSERT_ERROR(g < 256, "Green value must be in the range [0, 255]");
    KIT_ASSERT_ERROR(b < 256, "Blue value must be in the range [0, 255]");
    KIT_ASSERT_ERROR(a < 256, "Alpha value must be in the range [0, 255]");
}

color::color(const color &rgb, float a) : rgba(glm::vec3(rgb.rgba), a)
{
    KIT_ASSERT_ERROR(a <= 1.f && a >= 0.f, "Alpha value must be in the range [0, 1]");
}
color::color(const color &rgb, std::uint32_t a) : rgba(glm::vec3(rgb.rgba), to_float(a))
{
    KIT_ASSERT_ERROR(a < 256, "Alpha value must be in the range [0, 255]");
}

std::uint8_t color::r() const
{
    return to_int(rgba.r);
}
std::uint8_t color::g() const
{
    return to_int(rgba.g);
}
std::uint8_t color::b() const
{
    return to_int(rgba.b);
}
std::uint8_t color::a() const
{
    return to_int(rgba.a);
}

void color::r(std::uint8_t r)
{
    rgba.r = to_float(r);
}
void color::g(std::uint8_t g)
{
    rgba.g = to_float(g);
}
void color::b(std::uint8_t b)
{
    rgba.b = to_float(b);
}
void color::a(std::uint8_t a)
{
    rgba.a = to_float(a);
}

std::uint32_t color::to_hex_int(const bool alpha) const
{
    if (alpha)
        return (r() << 24) | (g() << 16) | (b() << 8) | a();
    return (r() << 16) | (g() << 8) | b();
}
std::string color::to_hex_str(const bool alpha) const
{
    std::stringstream ss;
    ss << std::hex << to_hex_int(alpha);
    std::string hex = ss.str();
    if (alpha)
        while (hex.size() < 8)
            hex = "0" + hex;
    else
        while (hex.size() < 6)
            hex = "0" + hex;
    return hex;
}

color color::from_hex(const std::uint32_t hex, const bool alpha)
{
    if (alpha)
        return {hex >> 24, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF};
    return {hex >> 16, (hex >> 8) & 0xFF, hex & 0xFF};
}
color color::from_hex(const std::string &hex, const bool alpha)
{
    std::uint32_t val;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> val;
    return from_hex(val, alpha);
}

const float *color::ptr() const
{
    return glm::value_ptr(rgba);
}
float *color::ptr()
{
    return glm::value_ptr(rgba);
}

color::operator const glm::vec4 &() const
{
    return rgba;
}
color::operator const glm::vec3 &() const
{
    return rgb;
}

color &color::operator+=(const color &rhs)
{
    rgb = glm::clamp(rgb + rhs.rgb, 0.f, 1.f);
    return *this;
}
color &color::operator-=(const color &rhs)
{
    rgb = glm::clamp(rgb - rhs.rgb, 0.f, 1.f);
    return *this;
}

color &color::operator*=(const color &rhs)
{
    rgb = glm::clamp(rgb * rhs.rgb, 0.f, 1.f);
    return *this;
}

color &color::operator/=(const color &rhs)
{
    rgb = glm::clamp(rgb / rhs.rgb, 0.f, 1.f);
    return *this;
}

color operator+(const color &lhs, const color &rhs)
{
    color result = lhs;
    result += rhs;
    return result;
}

color operator-(const color &lhs, const color &rhs)
{
    color result = lhs;
    result -= rhs;
    return result;
}

color operator*(const color &lhs, const color &rhs)
{
    color result = lhs;
    result *= rhs;
    return result;
}

color operator/(const color &lhs, const color &rhs)
{
    color result = lhs;
    result /= rhs;
    return result;
}

bool operator==(const color &lhs, const color &rhs)
{
    return lhs.rgba == rhs.rgba;
}
bool operator!=(const color &lhs, const color &rhs)
{
    return lhs.rgba != rhs.rgba;
}

} // namespace lynx