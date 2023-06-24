#ifndef LYNX_CONTEXT_HPP
#define LYNX_CONTEXT_HPP

#include "lynx/core.hpp"
#include <array>

namespace lynx
{
class window;
class device;

class context
{
  public:
    static inline constexpr std::size_t MAX_CONTEXTS = 16;

    static const context *create(window *win);
    static const context *current();

    static const context *get(const window *win);
    static void set(const window *win);
    static void set(const context *ctx);

    static std::size_t count();
    static bool empty();

    const ref<const device> &device() const;
    lynx::window *window() const;
    bool valid() const;

  private:
    lynx::window *m_window = nullptr;
};
} // namespace lynx

#endif