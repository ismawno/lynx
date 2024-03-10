#include "lynx/internal/pch.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/device.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/rendering/buffer.hpp"

namespace lynx
{
template <Dimension Dim>
window<Dim>::window(const std::uint32_t width, const std::uint32_t height, const char *name)
    : nameable(name), m_width(width), m_height(height)
{
    init();
    input_t::install_callbacks(this);

    add_render_system<point_render_system<Dim>>();
    add_render_system<line_render_system<Dim>>();
    add_render_system<line_strip_render_system<Dim>>();
    add_render_system<triangle_render_system<Dim>>();
    add_render_system<triangle_strip_render_system<Dim>>();

    if constexpr (std::is_same_v<Dim, dimension::two>)
        set_camera<orthographic2D>(pixel_aspect(), 5.f);
    else
        set_camera<perspective3D>(pixel_aspect(), glm::radians(60.f));
}

template <Dimension Dim> window<Dim>::~window()
{
    close();
}

template <Dimension Dim> void window<Dim>::init()
{
    KIT_CHECK_RETURN_VALUE(glfwInit(), GLFW_TRUE, CRITICAL, "GLFW failed to initialize")
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow((int)m_width, (int)m_height, name, nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);

    context_t::set(this);
    m_device = kit::make_ref<lynx::device>(m_window);
    m_renderer = kit::make_scope<renderer_t>(m_device, *this);
}

template <Dimension Dim> void window<Dim>::close()
{
    clear_render_data();
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

template <Dimension Dim> void window<Dim>::wait_for_device() const
{
    vkDeviceWaitIdle(m_device->vulkan_device());
}

template <Dimension Dim> bool window<Dim>::closed()
{
    if (!m_window)
        return true;
    if (glfwWindowShouldClose(m_window))
    {
        close();
        return true;
    }
    return false;
}

template <Dimension Dim> void window<Dim>::render(const VkCommandBuffer command_buffer) const
{
    KIT_PERF_PRETTY_FUNCTION()
    for (const auto &sys : m_render_systems)
        sys->render(command_buffer, *m_camera);
}

template <Dimension Dim> void window<Dim>::clear_render_data()
{
    KIT_PERF_FUNCTION()
    for (const auto &sys : m_render_systems)
        sys->clear_render_data();
    if constexpr (std::is_same_v<Dim, dimension::two>)
        render_system_t::s_z_offset_counter2D = 0;
}

template <Dimension Dim> bool window<Dim>::was_resized() const
{
    return m_resized;
}
template <Dimension Dim> void window<Dim>::complete_resize()
{
    m_resized = false;
}

template <Dimension Dim> void window<Dim>::make_context_current() const
{
    glfwMakeContextCurrent(m_window);
}

template <Dimension Dim> bool window<Dim>::maintain_camera_aspect_ratio() const
{
    return m_maintain_camera_aspect_ratio;
}
template <Dimension Dim> void window<Dim>::maintain_camera_aspect_ratio(const bool maintain)
{
    m_maintain_camera_aspect_ratio = maintain;
}

template <Dimension Dim> void window<Dim>::resize(const std::uint32_t width, const std::uint32_t height)
{
    m_width = width;
    m_height = height;
    m_resized = true;
}

template <Dimension Dim> const color &window<Dim>::clear_color() const
{
    return m_clear_color;
}
template <Dimension Dim> void window<Dim>::clear_color(const color &color)
{
    m_clear_color = color;
}

template <Dimension Dim> void window<Dim>::push_event(const event_t &ev)
{
    m_event_queue.push(ev);
}

template <Dimension Dim> event<Dim> window<Dim>::poll_event()
{
    if (m_event_queue.empty())
    {
        event_t ev;
        ev.empty = true;
        return ev;
    }
    const event_t ev = m_event_queue.front();
    m_event_queue.pop();
    return ev;
}

template <Dimension Dim> const renderer<Dim> &window<Dim>::renderer() const
{
    return *m_renderer;
}

template <Dimension Dim> const kit::ref<const lynx::device> &window<Dim>::device() const
{
    return m_device;
}
template <Dimension Dim>
void window<Dim>::draw(const std::vector<vertex_t> &vertices, const topology tplg, const transform_t &transform)
{
    render_system_from_topology<render_system_t>(tplg)->draw(vertices, transform);
}
template <Dimension Dim>
void window<Dim>::draw(const std::vector<vertex_t> &vertices, const std::vector<std::uint32_t> &indices,
                       const topology tplg, const transform_t &transform)
{
    render_system_from_topology<render_system_t>(tplg)->draw(vertices, indices, transform);
}
template <Dimension Dim> void window<Dim>::draw(const drawable_t &drawable)
{
    drawable.draw(*this);
}

template <Dimension Dim> GLFWwindow *window<Dim>::glfw_window() const
{
    return m_window;
}

template <Dimension Dim> std::uint32_t window<Dim>::screen_width() const
{
    return m_width;
}
template <Dimension Dim> std::uint32_t window<Dim>::screen_height() const
{
    return m_height;
}

template <Dimension Dim> std::uint32_t window<Dim>::pixel_width() const
{
    return m_renderer->swap_chain().width();
}
template <Dimension Dim> std::uint32_t window<Dim>::pixel_height() const
{
    return m_renderer->swap_chain().height();
}

template <Dimension Dim> float window<Dim>::screen_aspect() const
{
    return (float)m_width / (float)m_height;
}
template <Dimension Dim> float window<Dim>::pixel_aspect() const
{
    return m_renderer->swap_chain().extent_aspect_ratio();
}

template <Dimension Dim> VkExtent2D window<Dim>::extent() const
{
    return {m_width, m_height};
}

template <Dimension Dim> bool window<Dim>::should_close() const
{
    return !m_window || glfwWindowShouldClose(m_window);
}

template class window<dimension::two>;
template class window<dimension::three>;

} // namespace lynx