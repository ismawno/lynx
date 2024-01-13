#pragma once

#include "lynx/internal/dimension.hpp"
#include "kit/memory/ref.hpp"
#include <array>

namespace lynx
{
class device;

template <Dimension Dim> class context
{
  public:
    using window_t = window<Dim>;

    static bool valid();
    static window_t *window();
    static const kit::ref<const device> &device();
    static void set(window_t *win);

  private:
    static inline window_t *s_window = nullptr;
};

using context2D = context<dimension::two>;
using context3D = context<dimension::three>;
} // namespace lynx
