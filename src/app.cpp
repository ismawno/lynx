#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
app::app(window &win) : m_window(win)
{
}

app::~app()
{
    DBG_ASSERT_ERROR(m_terminated || !m_started,
                     "Application being destroyed has not been terminated properly with shutdown()")
}

void app::run()
{
    start();
    while (next_frame())
        ;
    shutdown();
}

void app::start()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot call start on a terminated app")
    DBG_ASSERT_ERROR(!m_started, "Cannot call start on a started app")
    on_start();
    m_started = true;
}

bool app::next_frame()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot fetch next frame on a terminated app")
    DBG_ASSERT_ERROR(m_started, "App must be started first by calling start() before fetching the next frame")
    if (m_window.closed())
        return false;
    m_window.poll_events();

    static float delta_time = 0.f;
    const auto start = std::chrono::high_resolution_clock::now();

    m_window.clear();
    on_update(delta_time);
    if (m_window.closed())
        return false;
    m_window.display();

    const auto end = std::chrono::high_resolution_clock::now();
    delta_time = std::chrono::duration<float, std::chrono::seconds::period>(end - start).count();
    return !m_window.closed();
}

void app::shutdown()
{
    DBG_ASSERT_ERROR(m_started, "Cannot terminate an app that has not been started")
    DBG_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")
    m_window.clear();
    on_shutdown();
    m_terminated = true;
}

app2D::app2D(const std::uint32_t width, const std::uint32_t height, const char *name)
    : app(m_window), m_window(width, height, name)
{
}

app3D::app3D(const std::uint32_t width, const std::uint32_t height, const char *name)
    : app(m_window), m_window(width, height, name)
{
}

} // namespace lynx