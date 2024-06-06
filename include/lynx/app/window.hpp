#pragma once

#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/renderer.hpp"
#include "lynx/rendering/swap_chain.hpp"
#include "lynx/app/input.hpp"
#include "lynx/drawing/drawable.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/internal/context.hpp"
#include "kit/memory/ptr/ref.hpp"
#include "kit/memory/ptr/scope.hpp"
#include "kit/interface/nameable.hpp"
#include "kit/utility/utils.hpp"
#include "kit/utility/type_constraints.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <queue>

namespace lynx
{
class device;
template <Dimension Dim> class window : kit::non_copyable, public kit::nameable
{
  public:
    using camera_t = typename Dim::camera_t;
    using vertex_t = vertex<Dim>;
    using drawable_t = drawable<Dim>;
    using render_system_t = render_system<Dim>;
    using renderer_t = renderer<Dim>;
    using event_t = event<Dim>;
    using transform_t = typename Dim::transform_t;
    using context_t = context<Dim>;
    using input_t = input<Dim>;

    struct specs
    {
        const char *name;
        std::uint32_t width;
        std::uint32_t height;
    };

    window(const specs &spc);
    ~window();

    color background_color = {0.01f, 0.01f, 0.01f, 1.f};

    std::uint32_t screen_width() const;
    std::uint32_t screen_height() const;

    std::uint32_t pixel_width() const;
    std::uint32_t pixel_height() const;

    float screen_aspect() const;
    float pixel_aspect() const;

    VkExtent2D extent() const;

    template <kit::Callable<VkCommandBuffer> F> bool display(F submission = [](VkCommandBuffer) {})
    {
        KIT_PERF_FUNCTION()
        if (VkCommandBuffer command_buffer = m_renderer->begin_frame())
        {
            if (m_maintain_camera_aspect_ratio)
                m_camera->keep_aspect_ratio(m_renderer->swap_chain().extent_aspect_ratio());
            m_camera->update_transformation_matrices();

            m_renderer->begin_swap_chain_render_pass(command_buffer, background_color);
            render(command_buffer);
            submission(command_buffer);

            m_renderer->end_swap_chain_render_pass(command_buffer);
            m_renderer->end_frame();

            clear_render_data();
            return true;
        }
        return false;
    }

    bool should_close() const;
    bool closed();
    void close();
    void wait_for_device() const;

    template <kit::DerivedFrom<render_system_t> T = render_system_t>
    const T *render_system_from_topology(topology tplg) const
    {
        return kit::const_get_casted_raw_ptr<T>(m_render_systems[(std::size_t)tplg]);
    }
    template <kit::DerivedFrom<render_system_t> T = render_system_t> T *render_system_from_topology(topology tplg)
    {
        return kit::get_casted_raw_ptr<T>(m_render_systems[(std::size_t)tplg]);
    }

    bool was_resized() const;
    void complete_resize();
    void make_context_current() const;

    bool maintain_camera_aspect_ratio() const;
    void maintain_camera_aspect_ratio(bool maintain);
    void resize(std::uint32_t width, std::uint32_t height);

    void push_event(const event_t &ev);
    event_t poll_event();

    const renderer_t &renderer() const;
    const kit::ref<const lynx::device> &device() const;

    void draw(const std::vector<vertex_t> &vertices, topology tplg, const transform_t &transform = {});
    void draw(const std::vector<vertex_t> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const transform_t &transform = {});
    void draw(const drawable_t &drawable);

    template <kit::DerivedFrom<camera_t> T = camera_t> const T *camera() const
    {
        return kit::const_get_casted_raw_ptr<T>(m_camera);
    }

    template <kit::DerivedFrom<camera_t> T = camera_t> T *camera()
    {
        return kit::get_casted_raw_ptr<T>(m_camera);
    }

    template <kit::DerivedFrom<camera_t> T, class... Args> T *set_camera(Args &&...args)
    {
        auto cam = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = cam.get();
        m_camera = std::move(cam);
        return ptr;
    }

    GLFWwindow *glfw_window() const;

    template <kit::DerivedFrom<render_system_t> T, class... Args> T *add_render_system(Args &&...args)
    {
        KIT_ASSERT_ERROR(!get_render_system<T>(), "A system with the provided type already exists")

        auto system = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        m_render_systems.push_back(std::move(system));
        return ptr;
    }

    template <kit::DerivedFrom<render_system_t> T> const T *get_render_system() const
    {
        for (const auto &rs : m_render_systems)
        {
            auto cast = dynamic_cast<const T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

    template <kit::DerivedFrom<render_system_t> T, typename B> T *get_render_system()
    {
        for (const auto &rs : m_render_systems)
        {
            auto cast = dynamic_cast<T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

  protected:
    bool m_maintain_camera_aspect_ratio = false;
    void clear_render_data();

  private:
    std::uint32_t m_width, m_height;
    GLFWwindow *m_window;

    kit::ref<const lynx::device> m_device;
    kit::scope<renderer_t> m_renderer;
    kit::scope<camera_t> m_camera;
    std::queue<event_t> m_event_queue;
    std::vector<kit::scope<render_system_t>> m_render_systems;

    bool m_resized = false;

    void init();
    void render(VkCommandBuffer command_buffer) const;
};

using window2D = window<dimension::two>;
using window3D = window<dimension::three>;

} // namespace lynx
