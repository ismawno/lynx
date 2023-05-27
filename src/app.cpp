#include "lynx/app.hpp"

namespace lynx
{
app::app(const std::uint32_t width, const std::uint32_t height, const char *name) : m_window(width, height, name)
{
}

void app::run() const
{
    while (!m_window.should_close())
    {
        glfwPollEvents();
    }
}
} // namespace lynx