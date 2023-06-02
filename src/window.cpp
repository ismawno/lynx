#include "lynx/pch.hpp"
#include "lynx/window.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/device.hpp"
#include "lynx/renderer.hpp"
#include "lynx/model.hpp"
#include "lynx/render_systems.hpp"

namespace lynx
{

window::window(const std::uint32_t width, const std::uint32_t height, const char *name)
    : m_width(width), m_height(height), m_name(name)
{
    init();
    load_models();
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
}

void window::create_surface(VkInstance instance, VkSurfaceKHR *surface) const
{
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
        throw bad_init("GLFW failed to initialize");
}

void window::load_models()
{
    const std::vector<model::vertex> vertices = {
        {{0.f, -0.25f}, {1.f, 0.f, 0.f}}, {{0.25f, 0.25f}, {0.f, 1.f, 0.f}}, {{-0.25f, 0.25f}, {0.f, 0.f, 1.f}}};
    m_model = make_scope<model>(m_device, vertices);
}

void window::poll_events()
{
    glfwPollEvents();
}

bool window::display()
{
    static line_render_system system{m_device, m_renderer->swap_chain_render_pass()};
    if (VkCommandBuffer command_buffer = m_renderer->begin_frame())
    {
        m_renderer->begin_swap_chain_render_pass(command_buffer);
        system.render(command_buffer, *m_model);
        m_renderer->end_swap_chain_render_pass(command_buffer);
        m_renderer->end_frame();
        return true;
    }
    return false;
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

const ref<const device> &window::gpu() const
{
    return m_device;
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