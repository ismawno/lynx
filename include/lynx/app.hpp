#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/window.hpp"

namespace lynx
{
class app
{
  public:
    app() = default;

    virtual void run() = 0;

  protected:
    void run_impl(const window &win);

  private:
    virtual void on_draw()
    {
    }

    app(const app &) = delete;
    app &operator=(const app &) = delete;
};

class app2D : public app
{
  public:
    app2D(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "Window 2D");

    void run() override;
    window2D &window();

  protected:
    window2D m_window;
};

class app3D : public app
{
  public:
    app3D(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "Window 3D");

    void run() override;
    window3D &window();

  protected:
    window3D m_window;
};

} // namespace lynx

#endif