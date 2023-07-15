#include "lynx/internal/pch.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/device.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/rendering/buffer.hpp"
#include "lynx/utility/context.hpp"

namespace lynx
{
window::window(const std::uint32_t width, const std::uint32_t height, const char *name)
    : m_width(width), m_height(height), m_name(name)
{
    init();
    input::install_callbacks(this);
}

window::~window()
{
    close();
}

void window::init()
{
    DBG_CHECK_RETURN_VALUE(glfwInit(), GLFW_TRUE, CRITICAL, "GLFW failed to initialize")
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow((int)m_width, (int)m_height, m_name, nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);

    context::create(this);
    m_device = kit::make_ref<lynx::device>(*this);
    m_renderer = kit::make_scope<lynx::renderer>(m_device, *this);
}

void window::create_surface(VkInstance instance, VkSurfaceKHR *surface) const
{
    DBG_CHECK_RETURN_VALUE(glfwCreateWindowSurface(instance, m_window, nullptr, surface), VK_SUCCESS, CRITICAL,
                           "Failed to create GLFW window surface")
}

bool window::display(const std::function<void(VkCommandBuffer)> &submission) const
{
    if (VkCommandBuffer command_buffer = m_renderer->begin_frame())
    {
        if (m_maintain_camera_aspect_ratio)
            m_camera->keep_aspect_ratio(m_renderer->swap_chain().extent_aspect_ratio());
        m_camera->update_transformation_matrices();

        m_renderer->begin_swap_chain_render_pass(command_buffer, m_clear_color);
        render(command_buffer);
        if (submission)
            submission(command_buffer);

        m_renderer->end_swap_chain_render_pass(command_buffer);
        m_renderer->end_frame();
        return true;
    }
    return false;
}

void window::clear()
{
    vkDeviceWaitIdle(m_device->vulkan_device());
    clear_render_data();
}

void window::close()
{
    clear_render_data();
    vkDeviceWaitIdle(m_device->vulkan_device());
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

bool window::closed()
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

void window::render(const VkCommandBuffer command_buffer) const
{
    for (const auto &sys : m_render_systems)
        sys->render(command_buffer, *m_camera);
}

void window::clear_render_data()
{
    for (const auto &sys : m_render_systems)
        sys->clear_render_data();
}

bool window::was_resized() const
{
    return m_frame_buffer_resized;
}
void window::complete_resize()
{
    m_frame_buffer_resized = false;
}

void window::make_context_current() const
{
    glfwMakeContextCurrent(m_window);
}

bool window::maintain_camera_aspect_ratio() const
{
    return m_maintain_camera_aspect_ratio;
}
void window::maintain_camera_aspect_ratio(const bool maintain)
{
    m_maintain_camera_aspect_ratio = maintain;
}

void window::frame_buffer_resize(const std::uint32_t width, const std::uint32_t height)
{
    m_width = width;
    m_height = height;
    m_frame_buffer_resized = true;
}

const glm::vec4 &window::clear_color() const
{
    return m_clear_color;
}
void window::clear_color(const glm::vec4 &color)
{
    m_clear_color = color;
}

void window::push_event(const event &ev)
{
    m_event_queue.push(ev);
}

event window::poll_event()
{
    if (m_event_queue.empty())
    {
        event ev;
        ev.empty = true;
        return ev;
    }
    const event ev = m_event_queue.front();
    m_event_queue.pop();
    return ev;
}

const renderer &window::renderer() const
{
    return *m_renderer;
}

const kit::ref<const lynx::device> &window::device() const
{
    return m_device;
}

GLFWwindow *window::glfw_window() const
{
    return m_window;
}

std::uint32_t window::width() const
{
    return m_width;
}
std::uint32_t window::height() const
{
    return m_height;
}
const char *window::name() const
{
    return m_name;
}

float window::aspect() const
{
    return (float)m_width / (float)m_height;
}
float window::swap_chain_aspect() const
{
    return m_renderer->swap_chain().extent_aspect_ratio();
}

VkExtent2D window::extent() const
{
    return {m_width, m_height};
}

bool window::should_close() const
{
    return !m_window || glfwWindowShouldClose(m_window);
}

window2D::window2D(std::uint32_t width, std::uint32_t height, const char *name) : window(width, height, name)
{
    set_camera<orthographic2D>(swap_chain_aspect(), 5.f);

    add_render_system<point_render_system2D>();
    add_render_system<line_render_system2D>();
    add_render_system<line_strip_render_system2D>();
    add_render_system<triangle_render_system2D>();
    add_render_system<triangle_strip_render_system2D>();
}

void window2D::draw(const std::vector<vertex2D> &vertices, const topology tplg, const transform2D &transform)
{
    render_system_as_topology<render_system2D>(tplg)->draw(vertices, transform);
}

void window2D::draw(const std::vector<vertex2D> &vertices, const std::vector<std::uint32_t> &indices,
                    const topology tplg, const transform2D &transform)
{
    render_system_as_topology<render_system2D>(tplg)->draw(vertices, indices, transform);
}

void window2D::draw(const drawable2D &drawable)
{
    drawable.draw(*this);
}

void window2D::clear_render_data()
{
    window::clear_render_data();
    render_system2D::reset_z_offset_counter();
}

window3D::window3D(std::uint32_t width, std::uint32_t height, const char *name) : window(width, height, name)
{
    set_camera<perspective3D>(swap_chain_aspect(), glm::radians(60.f));

    add_render_system<point_render_system3D>();
    add_render_system<line_render_system3D>();
    add_render_system<line_strip_render_system3D>();
    add_render_system<triangle_render_system3D>();
    add_render_system<triangle_strip_render_system3D>();
}

void window3D::draw(const std::vector<vertex3D> &vertices, const topology tplg, const transform3D &transform)
{
    render_system_as_topology<render_system3D>(tplg)->draw(vertices, transform);
}

void window3D::draw(const std::vector<vertex3D> &vertices, const std::vector<std::uint32_t> &indices,
                    const topology tplg, const transform3D &transform)
{
    render_system_as_topology<render_system3D>(tplg)->draw(vertices, indices, transform);
}

void window3D::draw(const drawable3D &drawable)
{
    drawable.draw(*this);
}

} // namespace lynx