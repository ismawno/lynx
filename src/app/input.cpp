#include "lynx/internal/pch.hpp"
#include "lynx/app/input.hpp"
#include "lynx/app/window.hpp"
#include "lynx/geometry/camera.hpp"

namespace lynx
{
template <Dimension Dim> void input<Dim>::poll_events()
{
    glfwPollEvents();
}

template <Dimension Dim> bool input<Dim>::key_pressed(const key kc)
{
    const window_t *win = context_t::window();
    KIT_ASSERT_ERROR(context_t::valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return false;
    return key_pressed(*win, kc);
}
template <Dimension Dim> bool input<Dim>::key_pressed(const window_t &win, const key kc)
{
    return glfwGetKey(win.glfw_window(), (int)kc) == GLFW_PRESS;
}

template <Dimension Dim> bool input<Dim>::key_released(const key kc)
{
    const window_t *win = context_t::window();
    KIT_ASSERT_ERROR(context_t::valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return false;
    return key_released(*win, kc);
}
template <Dimension Dim> bool input<Dim>::key_released(const window_t &win, const key kc)
{
    return glfwGetKey(win.glfw_window(), (int)kc) == GLFW_RELEASE;
}

template <Dimension Dim> bool input<Dim>::mouse_button_pressed(const mouse btn)
{
    const window_t *win = context_t::window();
    KIT_ASSERT_ERROR(context_t::valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return false;
    return mouse_button_pressed(*win, btn);
}
template <Dimension Dim> bool input<Dim>::mouse_button_pressed(const window_t &win, const mouse btn)
{
    return glfwGetMouseButton(win.glfw_window(), (int)btn);
}

template <Dimension Dim> glm::vec2 input<Dim>::mouse_position()
{
    static glm::vec2 screen_mouse{0.f};
    const window_t *win = context_t::window();
    KIT_ASSERT_ERROR(context_t::valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return screen_mouse;

    double x, y;
    glfwGetCursorPos(win->glfw_window(), &x, &y);
    screen_mouse = {2.f * (float)x / (float)win->screen_width() - 1.f,
                    2.f * (float)y / (float)win->screen_height() - 1.f};
    return screen_mouse;
}

template <Dimension Dim> const char *input<Dim>::key_name(const key kc)
{
    return glfwGetKeyName((int)kc, 0);
}

template <typename T> static T *from_glfw(GLFWwindow *win)
{
    return (T *)glfwGetWindowUserPointer(win);
}

template <Dimension Dim> static void window_resize_callback(GLFWwindow *gwindow, const int width, const int height)
{
    event<Dim> ev;
    ev.type = event<Dim>::WINDOW_RESIZED;

    window<Dim> *win = from_glfw<window<Dim>>(gwindow);

    ev.window.old_width = win->screen_width();
    ev.window.old_height = win->screen_height();

    ev.window.new_width = (std::uint32_t)width;
    ev.window.new_height = (std::uint32_t)height;

    win->resize(ev.window.new_width, ev.window.new_height);
    win->push_event(ev);
}

template <Dimension Dim>
static void key_callback(GLFWwindow *gwindow, const int key, const int scancode, const int action, const int mods)
{
    event<Dim> ev;
    switch (action)
    {
    case GLFW_PRESS:
        ev.type = event<Dim>::KEY_PRESSED;
        break;
    case GLFW_RELEASE:
        ev.type = event<Dim>::KEY_RELEASED;
        break;
    case GLFW_REPEAT:
        ev.type = event<Dim>::KEY_REPEAT;
        break;
    default:
        break;
    }
    ev.key = (typename input<Dim>::key)key;
    from_glfw<window<Dim>>(gwindow)->push_event(ev);
}

template <Dimension Dim> static void cursor_position_callback(GLFWwindow *gwindow, const double xpos, const double ypos)
{
    event<Dim> ev;
    ev.type = event<Dim>::MOUSE_MOVED;
    ev.mouse.position = {(float)xpos, (float)ypos};
    from_glfw<window<Dim>>(gwindow)->push_event(ev);
}

template <Dimension Dim>
static void mouse_button_callback(GLFWwindow *gwindow, const int button, const int action, const int mods)
{
    event<Dim> ev;
    ev.type = action == GLFW_PRESS ? event<Dim>::MOUSE_PRESSED : event<Dim>::MOUSE_RELEASED;
    ev.mouse.button = (typename input<Dim>::mouse)button;
    from_glfw<window<Dim>>(gwindow)->push_event(ev);
}

template <Dimension Dim> static void scroll_callback(GLFWwindow *gwindow, double xoffset, double yoffset)
{
    event<Dim> ev;
    ev.type = event<Dim>::SCROLLED;
    ev.scroll_offset = {(float)xoffset, (float)yoffset};
    from_glfw<window<Dim>>(gwindow)->push_event(ev);
}

template <Dimension Dim> void input<Dim>::install_callbacks(window_t *win)
{
    glfwSetWindowSizeCallback(win->glfw_window(), window_resize_callback<Dim>);
    glfwSetKeyCallback(win->glfw_window(), key_callback<Dim>);
    glfwSetCursorPosCallback(win->glfw_window(), cursor_position_callback<Dim>);
    glfwSetMouseButtonCallback(win->glfw_window(), mouse_button_callback<Dim>);
    glfwSetScrollCallback(win->glfw_window(), scroll_callback<Dim>);
}

template class input<dimension::two>;
template class input<dimension::three>;
} // namespace lynx