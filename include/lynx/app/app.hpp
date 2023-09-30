#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/app/window.hpp"
#include "lynx/app/layer.hpp"
#include "lynx/internal/context.hpp"
#include "lynx/internal/dimension.hpp"
#include "kit/profile/clock.hpp"
#ifdef LYNX_ENABLE_IMGUI
#include <imgui.h>
#ifdef LYNX_ENABLE_IMPLOT
#include <implot.h>
#endif
#endif

namespace lynx
{
template <typename Dim> class app : kit::non_copyable
{
  public:
    using window_t = typename Dim::window_t;
    using layer_t = layer<Dim>;
    using context_t = context<Dim>;
    using input_t = input<Dim>;
    using event_t = event<Dim>;

    app() = default;
    virtual ~app();

    void run();

    void start();
    bool next_frame();
    void shutdown();

    kit::time frame_time() const;

    const std::vector<kit::scope<layer_t>> &layers() const;

    std::uint32_t framerate_cap() const;
    void limit_framerate(std::uint32_t framerate);

    template <typename T, class... Args> T *push_layer(Args &&...args)
    {
        static_assert(std::is_base_of_v<layer_t, T>, "Type must inherit from layer class");
        KIT_ASSERT_ERROR(!m_terminated, "Cannot push layers to a terminated app")

        context_t::set(m_window.get());
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

    template <typename T = layer_t> kit::scope<T> pop_layer(const std::string &name)
    {
        static_assert(std::is_base_of_v<layer_t, T>, "Type must inherit from layer class");
        KIT_ASSERT_ERROR(!m_terminated, "Cannot pop layers to a terminated app")

        context_t::set(m_window.get());

        for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
            if ((*it)->id == name)
            {
                (*it)->on_detach();
                kit::scope<T> to_remove;
                if constexpr (std::is_same_v<T, layer_t>)
                    to_remove = std::move(*it);
                else
                    to_remove = kit::scope<T>(dynamic_cast<T *>(it->release()));
                m_layers.erase(it);
                return to_remove;
            }

        return nullptr;
    }

    template <typename T> kit::scope<T> pop_layer(const T *ly)
    {
        static_assert(std::is_base_of_v<layer_t, T>, "Type must inherit from layer class");
        return pop_layer(ly->id);
    }

    template <typename T = window_t> const T *window() const
    {
        return kit::const_get_casted_raw_ptr<T>(m_window);
    }

    template <typename T = window_t> T *window()
    {
        return kit::get_casted_raw_ptr<T>(m_window);
    }

    template <typename T, class... Args> T *set_window(Args &&...args)
    {
        static_assert(std::is_base_of_v<window_t, T>, "Window type must inherit from window");
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

    std::vector<kit::scope<layer_t>> m_layers;
    kit::scope<window_t> m_window;
    kit::time m_frame_time;
    kit::time m_min_frame_time;

#ifdef LYNX_ENABLE_IMGUI
    VkDescriptorPool m_imgui_pool;
    ImGuiContext *m_imgui_context;
#ifdef LYNX_ENABLE_IMPLOT
    ImPlotContext *m_implot_context;
#endif
#endif

    virtual void on_start()
    {
    }
    virtual void on_late_start()
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
    virtual void on_late_shutdown()
    {
    }
    virtual bool on_event(const event_t &ev)
    {
        return false;
    }
    virtual void on_late_event(const event_t &ev)
    {
    }

#ifdef LYNX_ENABLE_IMGUI
    void imgui_init();
    void imgui_begin_render();
    void imgui_end_render();
    void imgui_submit_command(VkCommandBuffer command_buffer);
    void imgui_shutdown();

#endif
};

using app2D = app<dimension::two>;
using app3D = app<dimension::three>;

} // namespace lynx

#endif