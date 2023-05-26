#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/window.hpp"

namespace lynx
{
class app
{
  public:
    app(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "Window");

    void run();

    window m_window;
};
} // namespace lynx

#endif