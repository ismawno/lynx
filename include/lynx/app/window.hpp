#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#include "lynx/rendering/render_system.hpp"
#include "lynx/internal/core.hpp"
#include "lynx/rendering/renderer.hpp"
#include "lynx/rendering/swap_chain.hpp"
#include "lynx/app/input.hpp"
#include "lynx/drawing/drawable.hpp"

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
class buffer;

class camera2D;
class camera3D;

class orthographic2D;
class perspective3D;

class window : non_copyable
{
  public:
    window(std::uint32_t width, std::uint32_t height, const char *name);
    virtual ~window();

    std::uint32_t width() const;
    std::uint32_t height() const;
    const char *name() const;

    float aspect() const;
    float swap_chain_aspect() const;

    VkExtent2D extent() const;

    void create_surface(VkInstance instance, VkSurfaceKHR *surface) const;

    bool display(const std::function<void(VkCommandBuffer)> &submission = nullptr) const;
    void clear();

    bool should_close() const;
    bool closed();
    virtual void close();

    bool was_resized() const;
    void complete_resize();
    void make_context_current() const;

    bool maintain_camera_aspect_ratio() const;
    void maintain_camera_aspect_ratio(bool maintain);
    void frame_buffer_resize(std::uint32_t width, std::uint32_t height);

    const glm::vec4 &clear_color() const;
    void clear_color(const glm::vec4 &color);

    void push_event(const event &ev);
    event poll_event();

    const lynx::renderer &renderer() const;
    const ref<const lynx::device> &device() const;

    template <typename T = lynx::camera> const T *camera() const
    {
        static_assert(std::is_base_of<lynx::camera, T>::value, "Type must inherit from camera");
        if constexpr (std::is_same<T, lynx::camera>::value)
            return m_camera.get();
        else
            return dynamic_cast<const T *>(m_camera.get());
    }

    template <typename T = lynx::camera> T *camera()
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
        static_assert(std::is_base_of<lynx::render_system, B>::value, "Type must inherit from render_system");
        static_assert(
            std::is_base_of<B, T>::value,
            "Type must inherit from render_system2D or render_system3D, depending on the window you are using");

        DBG_ASSERT_ERROR(!render_system<T>(systems), "A system with the provided type already exists")
        auto system = make_scope<T>(std::forward<Args>(args)...);
        T *ptr = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        systems.push_back(std::move(system));
        return ptr;
    }

    template <typename T, typename B> const T *render_system(const std::vector<scope<B>> &systems) const
    {
        static_assert(std::is_base_of<lynx::render_system, B>::value, "Type must inherit from render_system");
        static_assert(
            std::is_base_of<B, T>::value,
            "Type must inherit from render_system2D or render_system3D, depending on the window you are using");
        for (const auto &rs : systems)
        {
            auto cast = dynamic_cast<const T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

    template <typename T, typename B> T *render_system(const std::vector<scope<B>> &systems)
    {
        static_assert(std::is_base_of<lynx::render_system, B>::value, "Type must inherit from render_system");
        static_assert(
            std::is_base_of<B, T>::value,
            "Type must inherit from render_system2D or render_system3D, depending on the window you are using");
        for (const auto &rs : systems)
        {
            auto cast = dynamic_cast<T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

  protected:
    bool m_maintain_camera_aspect_ratio = false;

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    ref<const lynx::device> m_device;
    scope<lynx::renderer> m_renderer;
    scope<lynx::camera> m_camera;
    std::queue<event> m_event_queue;

    bool m_frame_buffer_resized = false;
    glm::vec4 m_clear_color = {0.01f, 0.01f, 0.01f, 1.f};

    void init();
    virtual void render(VkCommandBuffer command_buffer) const = 0;
    virtual void clear_render_data() = 0;
};

class window2D : public window
{
  public:
    window2D(std::uint32_t width, std::uint32_t height, const char *name);
    virtual ~window2D() override;

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        return window::add_render_system<T>(m_render_systems, std::forward<Args>(args)...);
    }

    template <typename T> const T *render_system() const
    {
        return window::render_system<T>(m_render_systems);
    }

    template <typename T> T *render_system()
    {
        return window::render_system<T>(m_render_systems);
    }

    const render_system2D &render_system(topology tplg) const; // BRING ALL OF THIS TO BASE
    render_system2D &render_system(topology tplg);

    void draw(const std::vector<vertex2D> &vertices, topology tplg, const transform2D &transform = {});
    void draw(const std::vector<vertex2D> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const transform2D &transform = {});
    void draw(const drawable2D &drawable);

    void close() override;

    template <typename T = camera2D> const T *camera() const
    {
        static_assert(std::is_base_of<camera2D, T>::value, "Type must inherit from camera2D");
        return window::camera<T>();
    }

    template <typename T = camera2D> T *camera()
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
    void clear_render_data() override;
};

class window3D : public window
{
  public:
    window3D(std::uint32_t width, std::uint32_t height, const char *name);
    virtual ~window3D() override;

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        return window::add_render_system<T>(m_render_systems, std::forward<Args>(args)...);
    }

    template <typename T> const T *render_system() const
    {
        return window::render_system<T>(m_render_systems);
    }

    template <typename T> T *render_system()
    {
        return window::render_system<T>(m_render_systems);
    }

    const render_system3D &render_system(topology tplg) const;
    render_system3D &render_system(topology tplg);

    void draw(const std::vector<vertex3D> &vertices, topology tplg, const transform3D &transform = {});
    void draw(const std::vector<vertex3D> &vertices, const std::vector<std::uint32_t> &indices, topology tplg,
              const transform3D &transform = {});
    void draw(const drawable3D &drawable);

    void close() override;

    template <typename T = camera3D> const T *camera() const
    {
        static_assert(std::is_base_of<camera3D, T>::value, "Type must inherit from camera3D");
        return window::camera<T>();
    }

    template <typename T = camera3D> T *camera()
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
    void clear_render_data() override;
};
} // namespace lynx

#endif