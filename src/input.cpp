#include "lynx/pch.hpp"
#include "lynx/input.hpp"
#include "lynx/window.hpp"

namespace lynx::input
{
static const window *active_window()
{
    for (const window *win : window::active_windows())
        if (glfwGetWindowAttrib(win->glfw_window(), GLFW_FOCUSED))
            return win;
    return nullptr;
}

bool key_pressed(const key_code::key kc)
{
    const window *win = active_window();
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
} // namespace lynx::input