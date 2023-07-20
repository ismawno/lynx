#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/app/window.hpp"
#include "lynx/app/layer.hpp"
#include "lynx/utility/context.hpp"
#include "kit/profile/clock.hpp"

namespace lynx
{
class app : kit::non_copyable
{
  public:
    app() = default;
    virtual ~app();

    void run();

    void start();
    bool next_frame();
    void shutdown();

    std::uint32_t framerate_cap() const;
    void limit_framerate(std::uint32_t framerate);

    template <typename T, class... Args> kit::ref<T> push_layer(Args &&...args)
    {
        static_assert(std::is_base_of<layer, T>::value, "Type must inherit from layer class");
        KIT_ASSERT_ERROR(!m_terminated, "Cannot push layers to a terminated app")

        context::set(m_window.get());
        auto ly = kit::make_ref<T>(std::forward<Args>(args)...);

        ly->m_parent = this;
        m_layers.emplace_back(ly)->on_attach();
        return ly;
    }

    bool pop_layer(const layer *ly);

    template <typename T = lynx::window> const T *window() const
    {
        static_assert(std::is_base_of<lynx::window, T>::value, "Type must inherit from window class");
        if constexpr (std::is_same<T, lynx::window>::value)
            return m_window.get();
        else
            return dynamic_cast<const T *>(m_window.get());
    }

    template <typename T = lynx::window> T *window()
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
        auto win = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = win.get();
        m_window = std::move(win);
        return ptr;
    }

  private:
    bool m_started = false;
    bool m_terminated = false;
    bool m_to_finish_next_frame = false;
    bool m_ongoing_frame = false;
    float m_min_frame_seconds = 0.f;

    std::vector<kit::ref<layer>> m_layers;
    kit::scope<lynx::window> m_window;
    kit::clock m_frame_clock;

    virtual void on_start()
    {
    }
    virtual void on_update(float ts)
    {
    }
    virtual void on_late_update(float ts)
    {
    }
    virtual void on_render(float ts)
    {
    }
    virtual void on_late_render(float ts)
    {
    }
    virtual void on_shutdown()
    {
    }
    virtual bool on_event(const event &ev)
    {
        return false;
    }
};

class app2D : public app
{
  public:
    app2D(std::uint32_t width = 800, std::uint32_t height = 600, const char *name = "App 2D");

    template <typename T = window2D> const T *window() const
    {
        static_assert(std::is_base_of<lynx::window2D, T>::value, "Window type must inherit from window2D");
        return app::window<T>();
    }

    template <typename T = window2D> T *window()
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

    template <typename T = window3D> const T *window() const
    {
        static_assert(std::is_base_of<lynx::window3D, T>::value, "Window type must inherit from window3D");
        return app::window<T>();
    }

    template <typename T = window3D> T *window()
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