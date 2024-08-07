#pragma once

#include "lynx/app/window.hpp"
#include "lynx/app/layer.hpp"
#include "lynx/internal/context.hpp"
#include "lynx/internal/dimension.hpp"
#include "kit/profiling/clock.hpp"
#include "kit/serialization/yaml/serializer.hpp"
#include "kit/utility/type_constraints.hpp"

#ifdef LYNX_ENABLE_IMGUI
#include <imgui.h>
#ifdef LYNX_ENABLE_IMPLOT
#include <implot.h>
#endif
#endif

namespace lynx
{
template <Dimension Dim> class app : kit::non_copyable, public kit::yaml::serializable, public kit::yaml::deserializable
{
  public:
    using window_t = window<Dim>;
    using layer_t = layer<Dim>;
    using context_t = context<Dim>;
    using input_t = input<Dim>;
    using event_t = event<Dim>;

    enum class state
    {
        STARTING,
        EVENT_PROCESSING,
        UPDATING,
        RENDERING,
        SHUTTING_DOWN,
        NONE
    };

    app(const typename window_t::specs &spc);
    virtual ~app();

    void run();

    void start();
    bool next_frame();
    void shutdown();

    const char *name() const;
    state current_state() const;

    const window_t *window() const;
    window_t *window();

    kit::perf::time frame_time() const;
    kit::perf::time update_time() const;
    kit::perf::time render_time() const;

    auto begin() const
    {
        return m_layers.begin();
    }
    auto end() const
    {
        return m_layers.end();
    }

    const layer_t &operator[](std::size_t index) const;
    layer_t &operator[](std::size_t index);

    const layer_t *operator[](const std::string &name) const;
    layer_t *operator[](const std::string &name);

    std::uint32_t framerate() const;
    std::uint32_t framerate_cap() const;
    void limit_framerate(std::uint32_t framerate);

    template <kit::DerivedFrom<layer_t> L, class... Args> L *push_layer(Args &&...args)
    {
        KIT_ASSERT_ERROR(!m_terminated, "Cannot push layers to a terminated app")

        context_t::set(m_window.get());
        auto ly = kit::make_scope<L>(std::forward<Args>(args)...);
        L *ptr = ly.get();
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

    template <kit::DerivedFrom<layer_t> L = layer_t> kit::scope<L> pop_layer(const std::string &name)
    {
        KIT_ASSERT_ERROR(!m_terminated, "Cannot pop layers to a terminated app")

        context_t::set(m_window.get());

        for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
            if ((*it)->id == name)
            {
                (*it)->on_detach();
                kit::scope<L> to_remove;
                if constexpr (std::is_same_v<L, layer_t>)
                    to_remove = std::move(*it);
                else
                    to_remove = kit::scope<L>(dynamic_cast<L *>(it->release()));
                m_layers.erase(it);
                return to_remove;
            }

        return nullptr;
    }

    template <kit::DerivedFrom<layer_t> T> kit::scope<T> pop_layer(const T *ly)
    {
        return pop_layer(ly->id);
    }

#ifdef KIT_USE_YAML_CPP
    virtual YAML::Node encode() const override;
    virtual bool decode(const YAML::Node &node) override;
#endif

  private:
    bool m_started = false;
    bool m_terminated = false;
    bool m_to_finish_next_frame = false;
    bool m_ongoing_frame = false;

    std::vector<kit::scope<layer_t>> m_layers;
    kit::scope<window_t> m_window;
    kit::perf::time m_frame_time;
    kit::perf::time m_update_time;
    kit::perf::time m_render_time;

    kit::perf::time m_min_frame_time;

    state m_state = state::NONE;

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
