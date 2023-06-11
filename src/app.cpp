#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
void app::run_impl(const window &win)
{
    while (!win.should_close())
    {
        win.poll_events();
        win.clear();
        on_draw();
        win.display();
    }
    win.clear();
}

app2D::app2D(const std::uint32_t width, const std::uint32_t height, const char *name) : m_window(width, height, name)
{
}

void app2D::run()
{
    run_impl(m_window);
}

window2D &app2D::window()
{
    return m_window;
}

app3D::app3D(const std::uint32_t width, const std::uint32_t height, const char *name) : m_window(width, height, name)
{
}

void app3D::run()
{
    run_impl(m_window);
}

window3D &app3D::window()
{
    return m_window;
}
} // namespace lynx