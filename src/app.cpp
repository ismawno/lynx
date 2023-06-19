#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
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
    if (m_window->closed())
        return false;
    m_ongoing_frame = true;

    input::poll_events();
    while (const event ev = m_window->poll_event())
        ;

    m_last_timestamp = m_current_timestamp;
    m_current_timestamp = std::chrono::high_resolution_clock::now();
    m_window->clear();

    const float delta_time =
        std::chrono::duration<float, std::chrono::seconds::period>(m_current_timestamp - m_last_timestamp).count();

    on_update(delta_time);
    for (const auto &ly : m_layers)
        ly->on_update(delta_time);

    const auto submission = [this](const VkCommandBuffer cmd) {
        for (const auto &ly : m_layers)
            ly->on_command_submission(cmd);
    };
    m_window->display(submission);

    m_ongoing_frame = false;
    return !m_window->closed() && !m_to_finish_next_frame;
}

void app::shutdown()
{
    if (m_ongoing_frame)
    {
        m_to_finish_next_frame = true;
        return;
    }
    DBG_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")

    on_shutdown();
    for (const auto &ly : m_layers)
        ly->on_detach();
    m_terminated = true;
}

bool app::pop_layer(const layer *ly)
{
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        if (it->get() == ly)
        {
            m_layers.erase(it);
            return true;
        }
    return false;
}

app2D::app2D(const std::uint32_t width, const std::uint32_t height, const char *name)

{
    set_window(width, height, name);
}

app3D::app3D(const std::uint32_t width, const std::uint32_t height, const char *name)

{
    set_window(width, height, name);
}

} // namespace lynx