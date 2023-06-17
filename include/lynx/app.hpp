#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/window.hpp"
#include "lynx/layer.hpp"

#include <chrono>

namespace lynx
{
class app
{
  public:
    app(window &win);
    virtual ~app();

    void run();

    void start();
    bool next_frame();
    void shutdown();

    template <typename T, class... Args> ref<T> push_layer(Args &&...args)
    {
        static_assert(std::is_base_of<layer, T>::value, "Type must inherit from layer class");
        auto ly = make_ref<T>(std::forward<Args>(args)...);
        m_layers.emplace_back(ly)->on_attach(this);
        return ly;
    }

    bool pop_layer(const layer *ly);

    lynx::window &window() const;

  protected:
  private:
    bool m_started = false;
    bool m_terminated = false;

    std::vector<ref<layer>> m_layers;

    std::chrono::steady_clock::time_point m_last_timestamp;
    std::chrono::steady_clock::time_point m_current_timestamp;
    lynx::window &m_window;

    virtual void on_start()
    {
    }
    virtual void on_update(float ts)
    {
    }
    virtual void on_shutdown()
    {
    }

    app(const app &) = delete;
    app &operator=(const app &) = delete;
};

class app2D : public app
{
  public:
    app2D(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "App 2D");

  protected:
    window2D m_window;
};

class app3D : public app
{
  public:
    app3D(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "App 3D");

  protected:
    window3D m_window;
};

} // namespace lynx

#endif