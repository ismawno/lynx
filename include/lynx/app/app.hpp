#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/app/window.hpp"
#include "lynx/app/layer.hpp"
#include "lynx/utility/context.hpp"
#include "kit/profile/clock.hpp"
#ifdef LYNX_ENABLE_IMGUI
#include <imgui.h>
#endif

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

    kit::time frame_time() const;

    const std::vector<kit::scope<layer>> &layers() const;

    std::uint32_t framerate_cap() const;
    void limit_framerate(std::uint32_t framerate);

    template <typename T, class... Args> T *push_layer(Args &&...args)
    {
        static_assert(std::is_base_of<layer, T>::value, "Type must inherit from layer class");
        KIT_ASSERT_ERROR(!m_terminated, "Cannot push layers to a terminated app")

        context::set(m_window.get());
        auto ly = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = ly.get();
#ifdef DEBUG
        for (const auto &old : m_layers)
        {
            KIT_ASSERT_ERROR(*old != *ly,
                             "Cannot add a layer with a name that already exists. Layer names act as identifiers")
        }
#endif

        ly->m_parent = this;
        m_layers.emplace_back(std::move(ly))->on_attach();
        return ptr;
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

    std::vector<kit::scope<layer>> m_layers;
    kit::scope<lynx::window> m_window;
    kit::time m_frame_time;
    kit::time m_min_frame_time;

#ifdef LYNX_ENABLE_IMGUI
    VkDescriptorPool m_imgui_pool;
    ImGuiContext *m_imgui_context;
#endif

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

#ifdef LYNX_ENABLE_IMGUI
    void imgui_init();
    void imgui_begin_render();
    void imgui_end_render();
    void imgui_submit_command(VkCommandBuffer command_buffer);
    void imgui_shutdown();

#endif
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