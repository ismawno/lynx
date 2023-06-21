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
    app() = default;
    virtual ~app();

    void run();

    void start();
    bool next_frame();
    void shutdown();

    template <typename T, class... Args> ref<T> push_layer(Args &&...args)
    {
        static_assert(std::is_base_of<layer, T>::value, "Type must inherit from layer class");
        auto ly = make_ref<T>(std::forward<Args>(args)...);

        ly->m_parent = this;
        m_layers.emplace_back(ly)->on_attach();
        return ly;
    }

    bool pop_layer(const layer *ly);

    template <typename T = window> T *window() const
    {
        static_assert(std::is_base_of<lynx::window, T>::value, "Type must inherit from window class");
        if constexpr (std::is_same<T, lynx::window>::value)
            return m_window.get();
        else
            return dynamic_cast<T *>(m_window.get());
    }

    template <typename T, class... Args> T *set_window(Args &&...args)
    {
        static_assert(std::is_base_of<lynx::window, T>::value, "Window type must inherit from window");
        auto win = make_scope<T>(std::forward<Args>(args)...);
        T *ptr = win.get();
        m_window = std::move(win);
        return ptr;
    }

  private:
    bool m_started = false;
    bool m_terminated = false;
    bool m_to_finish_next_frame = false;
    bool m_ongoing_frame = false;

    std::vector<ref<layer>> m_layers;

    std::chrono::steady_clock::time_point m_last_timestamp;
    std::chrono::steady_clock::time_point m_current_timestamp;
    scope<lynx::window> m_window;

    virtual void on_start()
    {
    }
    virtual void on_update(float ts)
    {
    }
    virtual void on_shutdown()
    {
    }
    virtual bool on_event(event ev)
    {
        return false;
    }

    app(const app &) = delete;
    app &operator=(const app &) = delete;
};

class app2D : public app
{
  public:
    app2D(std::uint32_t width = 800, std::uint32_t height = 600, const char *name = "App 2D");

    template <typename T = window2D> T *window() const
    {
        static_assert(std::is_base_of<lynx::window2D, T>::value, "Window type must inherit from window2D");
        return app::window<T>();
    }

    template <typename T = window2D, class... Args> T *set_window(Args &&...args)
    {
        static_assert(std::is_base_of<lynx::window2D, T>::value, "Window type must inherit from window2D");
        return app::set_window<T>(std::forward<Args>(args)...);
    }
};

class app3D : public app
{
  public:
    app3D(std::uint32_t width = 800, std::uint32_t height = 600, const char *name = "App 3D");

    template <typename T = window3D> T *window() const
    {
        static_assert(std::is_base_of<lynx::window3D, T>::value, "Window type must inherit from window3D");
        return app::window<T>();
    }

    template <typename T = window3D, class... Args> T *set_window(Args &&...args)
    {
        static_assert(std::is_base_of<lynx::window3D, T>::value, "Window type must inherit from window3D");
        return app::set_window<T>(std::forward<Args>(args)...);
    }
};

} // namespace lynx

#endif