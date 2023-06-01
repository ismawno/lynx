#include "lynx/pch.hpp"
#include "lynx/window.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/device.hpp"
#include "lynx/renderer.hpp"
#include "lynx/model.hpp"

#define VERTEX_SHADER_PATH LYNX_SHADER_PATH "shader.vert.spv"
#define FRAGMENT_SHADER_PATH LYNX_SHADER_PATH "shader.frag.spv"

namespace lynx
{
struct push_constant_data2D // Needs to be restructured
{
    glm::mat2 transform{1.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

window::window(const std::uint32_t width, const std::uint32_t height, const char *name)
    : m_width(width), m_height(height), m_name(name)
{
    init();
    load_models();
    create_pipeline_layout();
    create_pipeline();
}

window::~window()
{
    glfwDestroyWindow(m_window);
    vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
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
        {{0.f, -0.5f}, {1.f, 0.f, 0.f}}, {{0.5f, 0.5f}, {0.f, 1.f, 0.f}}, {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    m_model = make_scope<model>(m_device, vertices);
}

void window::create_pipeline_layout()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(push_constant_data2D);

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pSetLayouts = nullptr;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
    if (vkCreatePipelineLayout(m_device->vulkan_device(), &layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        throw bad_init("Failed to create pipeline layout");
}

void window::create_pipeline()
{
    DBG_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!")
    pipeline::config_info pip_config{};
    pipeline::config_info::default_config(pip_config);
    pip_config.render_pass = m_renderer->swap_chain_render_pass();
    pip_config.pipeline_layout = m_pipeline_layout;
    m_pipeline = make_scope<pipeline>(m_device, VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH, pip_config);
}

void window::poll_events()
{
    glfwPollEvents();
}

bool window::display()
{
    if (VkCommandBuffer command_buffer = m_renderer->begin_frame())
    {
        m_renderer->begin_swap_chain_render_pass(command_buffer);
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