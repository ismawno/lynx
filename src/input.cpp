#include "lynx/pch.hpp"
#include "lynx/input.hpp"
#include "lynx/window.hpp"
#include "lynx/camera.hpp"

namespace lynx::input
{
static const window *s_pushed_window = nullptr;
static const window *active_window()
{
    if (s_pushed_window)
        return s_pushed_window;
    for (const window *win : window::active_windows())
        if (glfwGetWindowAttrib(win->glfw_window(), GLFW_FOCUSED))
            return win;
    return nullptr;
}

void poll_events()
{
    glfwPollEvents();
}

bool key_pressed(const key::key_code kc)
{
    const window *win = active_window();
    if (!win)
        return false;
    return key_pressed(*win, kc);
}
bool key_pressed(const window &win, const key::key_code kc)
{
    return glfwGetKey(win.glfw_window(), (int)kc) == GLFW_PRESS;
}

glm::vec2 screen_mouse_position()
{
    static glm::vec2 pixel_mouse{0.f};
    const window *win = active_window();
    if (!win)
        return pixel_mouse;

    double x, y;
    glfwGetCursorPos(win->glfw_window(), &x, &y);
    pixel_mouse = {2.f * (float)x / (float)win->width() - 1.f, 2.f * (float)y / (float)win->height() - 1.f};
    return pixel_mouse;
}
glm::vec3 world_mouse_position(const float z_screen)
{
    static glm::vec3 world_mouse{0.f};
    const window *win = active_window();
    if (!win)
        return world_mouse;

    world_mouse = win->camera().screen_to_world(screen_mouse_position(), z_screen);
    return world_mouse;
}

const char *key_name(const key::key_code kc)
{
    return glfwGetKeyName((int)kc, 0);
}

void push_window(const window *win)
{
    DBG_ASSERT_ERROR(!s_pushed_window, "Another window has already been pushed. Forgot to call pop_window()?")
    s_pushed_window = win;
}

void pop_window()
{
    DBG_ASSERT_ERROR(s_pushed_window, "No window has been pushed.")
    s_pushed_window = nullptr;
}

static window *from_glfw(GLFWwindow *win)
{
    return (window *)glfwGetWindowUserPointer(win);
}

static void frame_buffer_resize_callback(GLFWwindow *gwindow, const int width, const int height)
{
    event ev;
    ev.empty = false;
    ev.type = event::WINDOW_RESIZE;

    window *win = from_glfw(gwindow);

    ev.window.old_width = win->width();
    ev.window.old_height = win->height();

    ev.window.new_width = (std::uint32_t)width;
    ev.window.new_height = (std::uint32_t)height;

    win->frame_buffer_resize(ev.window.new_width, ev.window.new_height);
    win->push_event(ev);
}

static void key_callback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods)
{
    event ev;
    ev.empty = false;
    ev.type = (event::action_type)action;
    ev.key = (key::key_code)key;
    from_glfw(window)->push_event(ev);
}

static void cursor_position_callback(GLFWwindow *window, const double xpos, const double ypos)
{
    event ev;
    ev.empty = false;
    ev.type = event::MOUSE_DRAGGED;
    ev.mouse.position = {(float)xpos, (float)ypos};
}

void install_callbacks(window *win)
{
    glfwSetFramebufferSizeCallback(win->glfw_window(), frame_buffer_resize_callback);
    glfwSetKeyCallback(win->glfw_window(), key_callback);
    glfwSetCursorPosCallback(win->glfw_window(), cursor_position_callback);
}
} // namespace lynx::input