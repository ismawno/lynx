#include "lynx/window.hpp"
#include "lynx/pch.hpp"

namespace lynx
{
window::window(const std::size_t width, const std::size_t height, const char *name)
    : m_width(width), m_height(height), m_name(name)
{
    init_window();
}

window::~window()
{
    glfwDestroyWindow(m_window);
}

class bad_glfw_init : public std::runtime_error
{
  public:
    bad_glfw_init();
    const virtual char *what() const noexcept override
    {
        return "GLFW failed to initialize";
    }
};

void window::init_window()
{
    if (glfwInit() != GLFW_TRUE)
        throw bad_glfw_init();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow((int)m_width, (int)m_height, m_name, nullptr, nullptr);
}

std::size_t window::width() const
{
    return m_width;
}
std::size_t window::height() const
{
    return m_height;
}

} // namespace lynx