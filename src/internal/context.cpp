#include "lynx/internal/pch.hpp"
#include "lynx/internal/context.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/device.hpp"

namespace lynx
{
static std::array<context, context::MAX_CONTEXTS> s_contexts;
static std::size_t s_active_contexts = 0;
static context *s_current = nullptr;

const context *context::create(lynx::window *win)
{
    DBG_ASSERT_CRITICAL(s_active_contexts < MAX_CONTEXTS, "Reached maximum context count limit of {0}", MAX_CONTEXTS)
    s_current = &s_contexts[s_active_contexts++];
    s_current->m_window = win;
    return s_current;
}

const context *context::current()
{
    DBG_ASSERT_ERROR(s_active_contexts > 0,
                     "No contexts have been created yet. Contexts are created automatically when creating windows")
    DBG_ASSERT_WARN(s_current->valid(), "Current context is not valid")
    return s_current;
}

const context *context::get(const lynx::window *win)
{
    DBG_ASSERT_ERROR(win, "Cannot get a context from a null window pointer")
    DBG_ASSERT_ERROR(
        s_active_contexts > 0,
        "No contexts have been created yet, which should not happen as a non null pointer of a window exists")
    for (std::size_t i = 0; i < s_active_contexts; i++)
        if (s_contexts[i].m_window == win)
            return &s_contexts[i];
    DBG_DEBUG("No context found for window {0}", win->name())
    return nullptr;
}

void context::set(const lynx::window *win)
{
    DBG_ASSERT_ERROR(win, "Cannot set a context from a null window pointer")
    DBG_ASSERT_ERROR(
        s_active_contexts > 0,
        "No contexts have been created yet, which should not happen as a non null pointer of a window exists")
    for (std::size_t i = 0; i < s_active_contexts; i++)
        if (s_contexts[i].m_window == win)
        {
            s_current = &s_contexts[i];
            return;
        }
    DBG_DEBUG("No context found for window {0}", win->name())
}

void context::set(const context *ctx)
{
    DBG_ASSERT_ERROR(ctx, "Cannot set a context from a null context pointer")
    set(ctx->m_window);
}

std::size_t context::count()
{
    return s_active_contexts;
}

bool context::empty()
{
    return s_active_contexts <= 0;
}

const ref<const device> &context::device() const
{
    return m_window->device();
}
window *context::window() const
{
    return m_window;
}

bool context::valid() const
{
    return m_window && !m_window->closed();
}
} // namespace lynx