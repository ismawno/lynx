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
    DBG_ASSERT_ERROR(!m_terminated, "Cannot call start on a terminated app")
    DBG_ASSERT_ERROR(!m_started, "Cannot call start on a started app")
    m_current_timestamp = std::chrono::high_resolution_clock::now();
    m_started = true;
    context::set(m_window.get());
    on_start();
}

bool app::next_frame()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot fetch next frame on a terminated app")
    DBG_ASSERT_ERROR(m_started, "App must be started first by calling start() before fetching the next frame")
    m_ongoing_frame = true;

    context::set(m_window.get());
    input::poll_events();
    if (m_window->closed())
    {
        m_ongoing_frame = false;
        return false;
    }

    while (const event ev = m_window->poll_event())
        if (!on_event(ev))
            for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
                if ((*it)->on_event(ev))
                    break;

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
    context::set(m_window.get());
    if (m_ongoing_frame)
    {
        m_to_finish_next_frame = true;
        return;
    }
    DBG_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")
    vkDeviceWaitIdle(m_window->device()->vulkan_device());

    on_shutdown();
    for (const auto &ly : m_layers)
        ly->on_detach();
    m_terminated = true;
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