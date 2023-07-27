#include "lynx/internal/pch.hpp"
#include "lynx/app/input.hpp"
#include "lynx/app/window.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/utility/context.hpp"

namespace lynx::input
{
void poll_events()
{
    glfwPollEvents();
}

bool key_pressed(const key kc)
{
    const window *win = context::current()->window();
    KIT_ASSERT_ERROR(context::current()->valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return false;
    return key_pressed(*win, kc);
}
bool key_pressed(const window &win, const key kc)
{
    return glfwGetKey(win.glfw_window(), (int)kc) == GLFW_PRESS;
}

bool mouse_button_pressed(const mouse btn)
{
    const window *win = context::current()->window();
    KIT_ASSERT_ERROR(context::current()->valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return false;
    return mouse_button_pressed(*win, btn);
}
bool mouse_button_pressed(const window &win, const mouse btn)
{
    return glfwGetMouseButton(win.glfw_window(), (int)btn);
}

glm::vec2 mouse_position()
{
    static glm::vec2 screen_mouse{0.f};
    const window *win = context::current()->window();
    KIT_ASSERT_ERROR(context::current()->valid(), "Trying to get input feedback with a non valid current context")
    if (!win)
        return screen_mouse;

    double x, y;
    glfwGetCursorPos(win->glfw_window(), &x, &y);
    screen_mouse = {2.f * (float)x / (float)win->screen_width() - 1.f,
                    2.f * (float)y / (float)win->screen_height() - 1.f};
    return screen_mouse;
}

const char *key_name(const key kc)
{
    return glfwGetKeyName((int)kc, 0);
}

static window *from_glfw(GLFWwindow *win)
{
    return (window *)glfwGetWindowUserPointer(win);
}

static void window_resize_callback(GLFWwindow *gwindow, const int width, const int height)
{
    event ev;
    ev.type = event::WINDOW_RESIZED;

    window *win = from_glfw(gwindow);

    ev.window.old_width = win->screen_width();
    ev.window.old_height = win->screen_height();

    ev.window.new_width = (std::uint32_t)width;
    ev.window.new_height = (std::uint32_t)height;

    win->resize(ev.window.new_width, ev.window.new_height);
    win->push_event(ev);
}

static void key_callback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods)
{
    event ev;
    switch (action)
    {
    case GLFW_PRESS:
        ev.type = event::KEY_PRESSED;
        break;
    case GLFW_RELEASE:
        ev.type = event::KEY_RELEASED;
        break;
    case GLFW_REPEAT:
        ev.type = event::KEY_REPEAT;
        break;
    default:
        break;
    }
    ev.key = (input::key)key;
    from_glfw(window)->push_event(ev);
}

static void cursor_position_callback(GLFWwindow *window, const double xpos, const double ypos)
{
    event ev;
    ev.type = event::MOUSE_MOVED;
    ev.mouse.position = {(float)xpos, (float)ypos};
    from_glfw(window)->push_event(ev);
}

static void mouse_button_callback(GLFWwindow *window, const int button, const int action, const int mods)
{
    event ev;
    ev.type = action == GLFW_PRESS ? event::MOUSE_PRESSED : event::MOUSE_RELEASED;
    ev.mouse.button = (mouse)button;
    from_glfw(window)->push_event(ev);
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    event ev;
    ev.type = event::SCROLLED;
    ev.scroll_offset = {(float)xoffset, (float)yoffset};
    from_glfw(window)->push_event(ev);
}

void install_callbacks(window *win)
{
    glfwSetWindowSizeCallback(win->glfw_window(), window_resize_callback);
    glfwSetKeyCallback(win->glfw_window(), key_callback);
    glfwSetCursorPosCallback(win->glfw_window(), cursor_position_callback);
    glfwSetMouseButtonCallback(win->glfw_window(), mouse_button_callback);
    glfwSetScrollCallback(win->glfw_window(), scroll_callback);
}
} // namespace lynx::input