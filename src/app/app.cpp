#include "lynx/internal/pch.hpp"
#include "lynx/app/app.hpp"
#include "lynx/geometry/camera.hpp"

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
    shutdown();
}

void app::start()
{
    KIT_ASSERT_ERROR(!m_terminated, "Cannot call start on a terminated app")
    KIT_ASSERT_ERROR(!m_started, "Cannot call start on a started app")
    m_current_timestamp = std::chrono::high_resolution_clock::now();
    m_started = true;
    context::set(m_window.get());
    on_start();
}

bool app::next_frame()
{
    KIT_ASSERT_ERROR(!m_terminated, "Cannot fetch next frame on a terminated app")
    KIT_ASSERT_ERROR(m_started, "App must be started first by calling start() before fetching the next frame")
    m_ongoing_frame = true;

    context::set(m_window.get());
    input::poll_events();
    if (m_window->closed())
    {
        m_ongoing_frame = false;
        return false;
    }

    m_window->wait_for_device();

    while (const event ev = m_window->poll_event())
        if (!on_event(ev))
            for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
                if ((*it)->on_event(ev))
                    break;

    m_last_timestamp = m_current_timestamp;
    m_current_timestamp = std::chrono::high_resolution_clock::now();

    const float delta_time =
        std::chrono::duration<float, std::chrono::seconds::period>(m_current_timestamp - m_last_timestamp).count();

    on_update(delta_time);
    for (const auto &ly : m_layers)
        ly->on_update(delta_time);
    on_late_update(delta_time);

    on_render(delta_time);
    for (const auto &ly : m_layers)
        ly->on_render(delta_time);
    on_late_render(delta_time);

    const auto submission = [this](const VkCommandBuffer cmd) {
        for (const auto &ly : m_layers)
            ly->on_command_submission(cmd);
    };
    KIT_CHECK_RETURN_VALUE(m_window->display(submission), true, CRITICAL,
                           "Display failed to get command buffer for new frame")

    if (delta_time < m_min_frame_seconds)
        std::this_thread::sleep_for(
            std::chrono::milliseconds((long long)((m_min_frame_seconds - delta_time) * 1000.f)));
    m_ongoing_frame = false;
    return !m_window->closed() && !m_to_finish_next_frame;
}

void app::shutdown()
{
    context::set(m_window.get());
    if (m_ongoing_frame)
    {
        m_to_finish_next_frame = true;
        return;
    }
    KIT_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")
    m_window->wait_for_device();

    on_shutdown();
    for (const auto &ly : m_layers)
        ly->on_detach();
    m_terminated = true;
}

std::uint32_t app::framerate_cap() const
{
    return (std::uint32_t)(1.f / m_min_frame_seconds);
}
void app::limit_framerate(std::uint32_t framerate)
{
    KIT_ASSERT_ERROR(framerate > 0, "Framerate must be greater than 0!")
    m_min_frame_seconds = 1.f / (float)framerate;
}

bool app::pop_layer(const layer *ly)
{
    context::set(m_window.get());
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        if (it->get() == ly)
        {
            (*it)->on_detach();
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