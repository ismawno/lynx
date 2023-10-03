#include "lynx/internal/pch.hpp"
#include "lynx/internal/context.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/device.hpp"

namespace lynx
{
template <typename Dim> bool context<Dim>::valid()
{
    return s_window != nullptr;
}

template <typename Dim> window<Dim> *context<Dim>::window()
{
    KIT_ASSERT_ERROR(valid(), "Currento context is not valid")
    return s_window;
}

template <typename Dim> const kit::ref<const device> &context<Dim>::device()
{
    KIT_ASSERT_ERROR(valid(), "Currento context is not valid")
    return s_window->device();
}

template <typename Dim> void context<Dim>::set(window_t *window)
{
    s_window = window;
}

template class context<dimension::two>;
template class context<dimension::three>;
} // namespace lynx