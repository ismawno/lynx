#include "lynx/window.hpp"
#include "lynx/pch.hpp"

namespace lynx
{
window::window(const std::size_t width, const std::size_t height, const char *name)
    : m_width(width), m_height(height), m_name(name)
{
}

} // namespace lynx