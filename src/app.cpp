#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
app::app(lynx::window &win) : m_window(win)
{
}

app::~app()
{
    if (!m_terminated)
        shutdown();
}

void app::run()
{
    start();
    while (next_frame())
        ;
}

void app::start()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot call start on a terminated app")
    DBG_ASSERT_ERROR(!m_started, "Cannot call start on a started app")
    m_current_timestamp = std::chrono::high_resolution_clock::now();
    m_started = true;
    on_start();
}

bool app::next_frame()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot fetch next frame on a terminated app")
    DBG_ASSERT_ERROR(m_started, "App must be started first by calling start() before fetching the next frame")
    if (m_window.closed())
        return false;

    glfwPollEvents();
    m_window.make_context_current();

    m_last_timestamp = m_current_timestamp;
    m_current_timestamp = std::chrono::high_resolution_clock::now();
    m_window.clear();

    const float delta_time =
        std::chrono::duration<float, std::chrono::seconds::period>(m_current_timestamp - m_last_timestamp).count();

    on_update(delta_time);
    for (const auto &ly : m_layers)
        ly->on_update(delta_time);

    if (m_window.closed())
        return false;

    const auto submission = [this](const VkCommandBuffer cmd) {
        for (const auto &ly : m_layers)
            ly->on_command_submission(cmd);
    };
    m_window.display(submission);

    return !m_window.closed();
}

void app::shutdown()
{
    DBG_ASSERT_ERROR(m_started, "Cannot terminate an app that has not been started")
    DBG_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")

    on_shutdown();
    for (const auto &ly : m_layers)
        ly->on_detach();
    m_window.close();
    m_terminated = true;
}

window &app::window() const
{
    return m_window;
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