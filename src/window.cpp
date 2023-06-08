#include "lynx/pch.hpp"
#include "lynx/window.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/render_systems.hpp"
#include "lynx/device.hpp"
#include "lynx/model.hpp"
#include "lynx/camera.hpp"

namespace lynx
{

window::window(const std::uint32_t width, const std::uint32_t height, const char *name)
    : m_width(width), m_height(height), m_name(name)
{
    init();
}

window::~window()
{
    glfwDestroyWindow(m_window);
    vkDeviceWaitIdle(m_device->vulkan_device());
}

void window::init()
{
    if (glfwInit() != GLFW_TRUE)
        throw bad_init("GLFW failed to initialize");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow((int)m_width, (int)m_height, m_name, nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, frame_buffer_resize_callback);
    m_device = make_ref<device>(*this);
    m_renderer = make_scope<renderer>(m_device, *this);

    add_render_system<point_render_system2D>();
    add_render_system<line_render_system2D>();
    add_render_system<line_strip_render_system2D>();
    add_render_system<triangle_render_system2D>();
    add_render_system<triangle_strip_render_system2D>();

    add_render_system<point_render_system3D>();
    add_render_system<line_render_system3D>();
    add_render_system<line_strip_render_system3D>();
    add_render_system<triangle_render_system3D>();
    add_render_system<triangle_strip_render_system3D>();
}

void window::draw(const std::vector<vertex2D> &vertices, const topology tplg, const transform2D &transform) const
{
    m_render_systems2D[(std::size_t)tplg]->draw(vertices, transform);
}
void window::draw(const std::vector<vertex3D> &vertices, const topology tplg, const transform3D &transform) const
{
    m_render_systems3D[(std::size_t)tplg]->draw(vertices, transform);
}

void window::draw(const drawable2D &drawable) const
{
    const topology top = drawable.primitive_topology();
    drawable.draw(*m_render_systems2D[(std::size_t)top]);
}
void window::draw(const drawable3D &drawable) const
{
    const topology top = drawable.primitive_topology();
    drawable.draw(*m_render_systems3D[(std::size_t)top]);
}

void window::create_surface(VkInstance instance, VkSurfaceKHR *surface) const
{
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
        throw bad_init("GLFW failed to initialize");
}

void window::poll_events()
{
    glfwPollEvents();
}

bool window::display()
{
    const float aspect = m_renderer->swap_chain().extent_aspect_ratio();
    // const camera cam{{-aspect, -1.f, -1.f}, {aspect, 1.f, 1.f}};
    const camera cam{glm::radians(50.f), aspect, 0.1f, 10.f};
    if (VkCommandBuffer command_buffer = m_renderer->begin_frame())
    {
        m_renderer->begin_swap_chain_render_pass(command_buffer);
        for (const auto &sys : m_render_systems2D)
            sys->render(command_buffer, cam);
        for (const auto &sys : m_render_systems3D)
            sys->render(command_buffer, cam);
        m_renderer->end_swap_chain_render_pass(command_buffer);
        m_renderer->end_frame();
        return true;
    }
    return false;
}

void window::clear()
{
    vkDeviceWaitIdle(m_device->vulkan_device());
    for (const auto &sys : m_render_systems2D)
        sys->clear_render_data();
    for (const auto &sys : m_render_systems3D)
        sys->clear_render_data();
}

void window::frame_buffer_resize_callback(GLFWwindow *gwindow, const int width, const int height)
{
    window *win = (window *)glfwGetWindowUserPointer(gwindow);
    win->m_width = (std::uint32_t)width;
    win->m_height = (std::uint32_t)height;
    win->m_frame_buffer_resized = true;
}

bool window::was_resized() const
{
    return m_frame_buffer_resized;
}
void window::complete_resize()
{
    m_frame_buffer_resized = false;
}

const device &window::gpu() const
{
    return *m_device;
}

std::uint32_t window::width() const
{
    return m_width;
}
std::uint32_t window::height() const
{
    return m_height;
}

VkExtent2D window::extent() const
{
    return {m_width, m_height};
}

bool window::should_close() const
{
    return glfwWindowShouldClose(m_window);
}

} // namespace lynx