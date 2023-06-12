#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#include "lynx/render_systems.hpp"
#include "lynx/core.hpp"
#include "lynx/renderer.hpp"
#include "lynx/swap_chain.hpp"
#include "lynx/primitives.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
class renderer;
class camera2D;
class camera3D;

class window
{
  public:
    window(std::uint32_t width, std::uint32_t height, const char *name);
    ~window();

    std::uint32_t width() const;
    std::uint32_t height() const;

    float aspect() const;
    float swap_chain_aspect() const;

    VkExtent2D extent() const;

    void create_surface(VkInstance instance, VkSurfaceKHR *surface) const;

    void poll_events() const;
    bool display() const;
    void clear() const;

    bool should_close() const;
    bool closed();
    void close();

    bool was_resized() const;
    void complete_resize();

    bool maintain_camera_aspect_ratio() const;
    void maintain_camera_aspect_ratio(bool maintain);

    template <typename T, typename B, class... Args>
    T *add_render_system_impl(std::vector<scope<B>> &systems, Args &&...args)
    {
        static_assert(std::is_base_of<render_system, B>::value,
                      "Can only use add_render_system with a type that inherits from render_system");
        static_assert(
            std::is_base_of<B, T>::value,
            "Type must inherit from render_system2D or render_system3D, depending on the window you are using");

        auto system = make_scope<T>(std::forward<Args>(args)...);
        T *ref = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        systems.push_back(std::move(system));
        return ref;
    }

    template <typename T, typename B> T *render_system_impl(std::vector<scope<B>> &systems) const noexcept
    {
        for (auto &rs : systems)
        {
            auto cast = dynamic_cast<T *>(rs.get());
            if (cast)
                return cast;
        }
        return nullptr;
    }

  protected:
    bool m_maintain_camera_aspect_ratio = true;

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    ref<const device> m_device;
    scope<renderer> m_renderer;

    bool m_frame_buffer_resized = false;

    void init();
    virtual void render(VkCommandBuffer command_buffer) const = 0;
    virtual void clear_render_data() const = 0;

    static void frame_buffer_resize_callback(GLFWwindow *gwindow, int width, int height);

    window(const window &) = delete;
    window &operator=(const window &) = delete;
};

class window2D : public window
{
  public:
    window2D(std::uint32_t width, std::uint32_t height, const char *name);

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        return add_render_system_impl<T>(m_render_systems, std::forward<Args>(args)...);
    }

    template <typename T> T *render_system() const noexcept
    {
        return render_system_impl<T>(m_render_systems);
    }

    void draw(const std::vector<vertex2D> &vertices, topology tplg, const transform2D &transform = {}) const;
    void draw(const drawable2D &drawable) const;

    camera2D &camera() const;
    template <typename T> T *camera_as() const
    {
        return dynamic_cast<T *>(m_camera.get());
    }
    template <typename T, class... Args> T *camera(Args &&...args)
    {
        static_assert(std::is_base_of<camera2D, T>::value, "Camera type must inherit from camera2D");
        auto cam = make_scope<T>(std::forward<Args>(args)...);
        T *ref = cam.get();
        m_camera = std::move(cam);
        return ref;
    }

  private:
    scope<camera2D> m_camera;
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
        return add_render_system_impl<T>(m_render_systems, std::forward<Args>(args)...);
    }

    template <typename T> T *render_system() const noexcept
    {
        return render_system_impl<T>(m_render_systems);
    }

    void draw(const std::vector<vertex3D> &vertices, topology tplg, const transform3D &transform = {}) const;
    void draw(const drawable3D &drawable) const;

    camera3D &camera() const;
    template <typename T> T *camera_as() const
    {
        return dynamic_cast<T *>(m_camera.get());
    }
    template <typename T, class... Args> T *camera(Args &&...args)
    {
        static_assert(std::is_base_of<camera3D, T>::value, "Camera type must inherit from camera3D");
        auto cam = make_scope<T>(std::forward<Args>(args)...);
        T *ref = cam.get();
        m_camera = std::move(cam);
        return ref;
    }

  private:
    scope<camera3D> m_camera;
    std::vector<scope<render_system3D>> m_render_systems;

    void render(VkCommandBuffer command_buffer) const override;
    void clear_render_data() const override;
};
} // namespace lynx

#endif