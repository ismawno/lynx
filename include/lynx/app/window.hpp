#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/renderer.hpp"
#include "lynx/rendering/swap_chain.hpp"
#include "lynx/app/input.hpp"
#include "lynx/drawing/drawable.hpp"
#include "lynx/drawing/color.hpp"
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
class renderer;

class camera2D;
class camera3D;

class orthographic2D;
class perspective3D;

class window : kit::non_copyable, public kit::nameable
{
  public:
    window(std::uint32_t width, std::uint32_t height, const char *name);
    virtual ~window();

    std::uint32_t screen_width() const;
    std::uint32_t screen_height() const;

    std::uint32_t pixel_width() const;
    std::uint32_t pixel_height() const;

    float screen_aspect() const;
    float pixel_aspect() const;

    VkExtent2D extent() const;

    void create_surface(VkInstance instance, VkSurfaceKHR *surface) const;

    bool display(const std::function<void(VkCommandBuffer)> &submission = nullptr);

    bool should_close() const;
    bool closed();
    void close();
    void wait_for_device() const;

    template <typename T = lynx::render_system> const T *render_system_from_topology(topology tplg) const
    {
        return kit::const_get_casted_raw_ptr<T>(m_render_systems[(std::size_t)tplg]);
    }
    template <typename T = lynx::render_system> T *render_system_from_topology(topology tplg)
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

    void push_event(const event &ev);
    event poll_event();

    const lynx::renderer &renderer() const;
    const kit::ref<const lynx::device> &device() const;

    template <typename T = lynx::camera> const T *camera() const
    {
        return kit::const_get_casted_raw_ptr<T>(m_camera);
    }

    template <typename T = lynx::camera> T *camera()
    {
        return kit::get_casted_raw_ptr<T>(m_camera);
    }

    template <typename T, class... Args> T *set_camera(Args &&...args)
    {
        static_assert(std::is_base_of_v<lynx::camera, T>, "Type must inherit from camera");
        auto cam = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = cam.get();
        m_camera = std::move(cam);
        return ptr;
    }

    GLFWwindow *glfw_window() const;

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        static_assert(std::is_base_of_v<lynx::render_system, T>, "Type must inherit from render_system");
        KIT_ASSERT_ERROR(!render_system<T>(), "A system with the provided type already exists")

        auto system = kit::make_scope<T>(std::forward<Args>(args)...);
        T *ptr = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        m_render_systems.push_back(std::move(system));
        return ptr;
    }

    template <typename T> const T *render_system() const
    {
        static_assert(std::is_base_of_v<lynx::render_system, T>, "Type must inherit from render_system");
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
        static_assert(std::is_base_of_v<lynx::render_system, T>, "Type must inherit from render_system");
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
    virtual void clear_render_data();

  private:
    std::uint32_t m_width, m_height;
    GLFWwindow *m_window;

    kit::ref<const lynx::device> m_device;
    kit::scope<lynx::renderer> m_renderer;
    kit::scope<lynx::camera> m_camera;
    std::queue<event> m_event_queue;
    std::vector<kit::scope<lynx::render_system>> m_render_systems;

    bool m_resized = false;
    color m_clear_color = {0.01f, 0.01f, 0.01f, 1.f};

    void init();
    void render(VkCommandBuffer command_buffer) const;
};

class window2D : public window
{
  public:
    window2D(std::uint32_t width, std::uint32_t height, const char *name);

    void draw(const std::vector<vertex2D> &vertices, topology tplg, const kit::transform2D &transform = {});
    void draw(const std::vector<vertex2D> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const kit::transform2D &transform = {});
    void draw(const drawable2D &drawable);

  private:
    void clear_render_data() override;
};

class window3D : public window
{
  public:
    window3D(std::uint32_t width, std::uint32_t height, const char *name);

    void draw(const std::vector<vertex3D> &vertices, topology tplg, const kit::transform3D &transform = {});
    void draw(const std::vector<vertex3D> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const kit::transform3D &transform = {});
    void draw(const drawable3D &drawable);
};
} // namespace lynx

#endif