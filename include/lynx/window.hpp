#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#include "lynx/render_systems.hpp"
#include "lynx/core.hpp"
#include "lynx/renderer.hpp"
#include "lynx/swap_chain.hpp"
#include "lynx/primitives.hpp"
#include "lynx/input.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <unordered_set>
#include <functional>
#include <queue>

namespace lynx
{
class device;
class renderer;
class buffer;

class camera2D;
class camera3D;

class orthographic2D;
class perspective3D;

class window
{
  public:
    window(std::uint32_t width, std::uint32_t height, const char *name);
    virtual ~window();

    std::uint32_t width() const;
    std::uint32_t height() const;

    float aspect() const;
    float swap_chain_aspect() const;

    VkExtent2D extent() const;

    void create_surface(VkInstance instance, VkSurfaceKHR *surface) const;

    bool display(const std::function<void(VkCommandBuffer)> &submission = nullptr) const;
    void clear() const;

    bool should_close() const;
    bool closed();
    void close();

    bool was_resized() const;
    void complete_resize();
    void make_context_current() const;

    bool maintain_camera_aspect_ratio() const;
    void maintain_camera_aspect_ratio(bool maintain);
    void frame_buffer_resize(std::uint32_t width, std::uint32_t height);

    void push_event(const event &ev);
    event poll_event();

    const lynx::renderer &renderer() const;
    const lynx::device &device() const;

    template <typename T = camera> T *camera() const
    {
        static_assert(std::is_base_of<lynx::camera, T>::value, "Type must inherit from camera");
        if constexpr (std::is_same<T, lynx::camera>::value)
            return m_camera.get();
        else
            return dynamic_cast<T *>(m_camera.get());
    }

    template <typename T, class... Args> T *set_camera(Args &&...args)
    {
        static_assert(std::is_base_of<lynx::camera, T>::value, "Type must inherit from camera");
        auto cam = make_scope<T>(std::forward<Args>(args)...);
        T *ptr = cam.get();
        m_camera = std::move(cam);
        return ptr;
    }

    GLFWwindow *glfw_window() const;

    template <typename T, typename B, class... Args>
    T *add_render_system(std::vector<scope<B>> &systems, Args &&...args)
    {
        static_assert(std::is_base_of<lynx::render_system, B>::value,
                      "Can only use add_render_system with a type that inherits from render_system");
        static_assert(
            std::is_base_of<B, T>::value,
            "Type must inherit from render_system2D or render_system3D, depending on the window you are using");

        auto system = make_scope<T>(std::forward<Args>(args)...);
        T *ptr = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        systems.push_back(std::move(system));
        return ptr;
    }

    template <typename T, typename B> T *render_system(std::vector<scope<B>> &systems) const noexcept
    {
        for (auto &rs : systems)
        {
            auto cast = dynamic_cast<T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

    static const std::unordered_set<const window *> active_windows();

  protected:
    bool m_maintain_camera_aspect_ratio = true;

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    ref<const lynx::device> m_device;
    scope<lynx::renderer> m_renderer;
    scope<lynx::camera> m_camera;
    std::queue<event> m_event_queue;

    static inline std::unordered_set<const window *> s_active_windows{};

    bool m_frame_buffer_resized = false;

    void init();
    virtual void render(VkCommandBuffer command_buffer) const = 0;
    virtual void clear_render_data() const = 0;

    window(const window &) = delete;
    window &operator=(const window &) = delete;
};

class window2D : public window
{
  public:
    window2D(std::uint32_t width, std::uint32_t height, const char *name);

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        return window::add_render_system<T>(m_render_systems, std::forward<Args>(args)...);
    }

    template <typename T> T *render_system() const noexcept
    {
        return window::render_system<T>(m_render_systems);
    }

    void draw(const std::vector<vertex2D> &vertices, topology tplg, const transform2D &transform = {}) const;
    void draw(const std::vector<vertex2D> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const transform2D &transform = {}) const;
    void draw(const drawable2D &drawable) const;

    template <typename T = camera2D> T *camera() const
    {
        static_assert(std::is_base_of<camera2D, T>::value, "Type must inherit from camera2D");
        return window::camera<T>();
    }

    template <typename T = orthographic2D, class... Args> T *set_camera(Args &&...args)
    {
        static_assert(std::is_base_of<camera2D, T>::value, "Type must inherit from camera2D");
        return window::set_camera<T>(std::forward<Args>(args)...);
    }

  private:
    std::vector<scope<render_system2D>> m_render_systems;

    void render(VkCommandBuffer command_buffer) const override;
    void clear_render_data() const override;
};

class window3D : public window
{
  public:
    window3D(std::uint32_t width, std::uint32_t height, const char *name);

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        return window::add_render_system<T>(m_render_systems, std::forward<Args>(args)...);
    }

    template <typename T> T *render_system() const noexcept
    {
        return window::render_system<T>(m_render_systems);
    }

    void draw(const std::vector<vertex3D> &vertices, topology tplg, const transform3D &transform = {}) const;
    void draw(const std::vector<vertex3D> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const transform3D &transform = {}) const;
    void draw(const drawable3D &drawable) const;

    template <typename T = camera3D> T *camera() const
    {
        static_assert(std::is_base_of<camera3D, T>::value, "Type must inherit from camera3D");
        return window::camera<T>();
    }

    template <typename T = perspective3D, class... Args> T *set_camera(Args &&...args)
    {
        static_assert(std::is_base_of<camera3D, T>::value, "Type must inherit from camera3D");
        return window::set_camera<T>(std::forward<Args>(args)...);
    }

  private:
    std::vector<scope<render_system3D>> m_render_systems;

    void render(VkCommandBuffer command_buffer) const override;
    void clear_render_data() const override;
};
} // namespace lynx

#endif