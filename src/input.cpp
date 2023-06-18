#include "lynx/pch.hpp"
#include "lynx/input.hpp"
#include "lynx/window.hpp"

namespace lynx::input
{
static const window *s_pushed_window = nullptr;
static const window *active_window()
{
    for (const window *win : window::active_windows())
        if (glfwGetWindowAttrib(win->glfw_window(), GLFW_FOCUSED))
            return win;
    return nullptr;
}

bool key_pressed(const key_code::key kc)
{
    const window *win = s_pushed_window ? s_pushed_window : active_window();
    if (!win)
        return false;
    return key_pressed(*win, kc);
}

bool key_pressed(const window &win, const key_code::key kc)
{
    return glfwGetKey(win.glfw_window(), (int)kc) == GLFW_PRESS;
}

const char *key_name(const key_code::key kc)
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
} // namespace lynx::input