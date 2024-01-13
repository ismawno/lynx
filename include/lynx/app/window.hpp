#pragma once

#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/renderer.hpp"
#include "lynx/rendering/swap_chain.hpp"
#include "lynx/app/input.hpp"
#include "lynx/drawing/drawable.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/internal/context.hpp"
#include "kit/memory/ref.hpp"
#include "kit/memory/scope.hpp"
#include "kit/interface/nameable.hpp"
#include "kit/utility/utils.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <functional>
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

    window(std::uint32_t width, std::uint32_t height, const char *name);
    ~window();

    std::uint32_t screen_width() const;
    std::uint32_t screen_height() const;

    std::uint32_t pixel_width() const;
    std::uint32_t pixel_height() const;

    float screen_aspect() const;
    float pixel_aspect() const;

    VkExtent2D extent() const;

    bool display(const std::function<void(VkCommandBuffer)> &submission = nullptr);

    bool should_close() const;
    bool closed();
    void close();
    void wait_for_device() const;

    template <typename T = render_system_t> const T *render_system_from_topology(topology tplg) const
    {
        return kit::const_get_casted_raw_ptr<T>(m_render_systems[(std::size_t)tplg]);
    }
    template <typename T = render_system_t> T *render_system_from_topology(topology tplg)
    {
        return kit::get_casted_raw_ptr<T>(m_render_systems[(std::size_t)tplg]);
    }

    bool was_resized() const;
    void complete_resize();
    void make_context_current() const;

    bool maintain_camera_aspect_ratio() const;
    void maintain_camera_aspect_ratio(bool maintain);
    void resize(std::uint32_t width, std::uint32_t height);

    const color &clear_color() const;
    void clear_color(const color &rgb);

    void push_event(const event_t &ev);
    event_t poll_event();

    const renderer_t &renderer() const;
    const kit::ref<const lynx::device> &device() const;

    void draw(const std::vector<vertex_t> &vertices, topology tplg, const transform_t &transform = {});
    void draw(const std::vector<vertex_t> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const transform_t &transform = {});
    void draw(const drawable_t &drawable);

    template <typename T = camera_t> const T *camera() const
    {
        return kit::const_get_casted_raw_ptr<T>(m_camera);
    }

    template <typename T = camera_t> T *camera()
    {
        return kit::get_casted_raw_ptr<T>(m_camera);
    }

    template <typename T, class... Args> T *set_camera(Args &&...args)
    {
        static_assert(std::is_base_of_v<camera_t, T>, "Type must inherit from camera");
        auto cam = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = cam.get();
        m_camera = std::move(cam);
        return ptr;
    }

    GLFWwindow *glfw_window() const;

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        static_assert(std::is_base_of_v<render_system_t, T>, "Type must inherit from render_system");
        KIT_ASSERT_ERROR(!render_system<T>(), "A system with the provided type already exists")

        auto system = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        m_render_systems.push_back(std::move(system));
        return ptr;
    }

    template <typename T> const T *render_system() const
    {
        static_assert(std::is_base_of_v<render_system_t, T>, "Type must inherit from render_system");
        for (const auto &rs : m_render_systems)
        {
            auto cast = dynamic_cast<const T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

    template <typename T, typename B> T *render_system()
    {
        static_assert(std::is_base_of_v<render_system_t, T>, "Type must inherit from render_system");
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
    color m_clear_color = {0.01f, 0.01f, 0.01f, 1.f};

    void init();
    void render(VkCommandBuffer command_buffer) const;
};

using window2D = window<dimension::two>;
using window3D = window<dimension::three>;

} // namespace lynx
