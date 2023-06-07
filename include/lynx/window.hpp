#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#include "lynx/render_systems.hpp"
#include "lynx/core.hpp"
#include "lynx/renderer.hpp"
#include "lynx/swap_chain.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
class renderer;
class window
{
  public:
    window(std::uint32_t width, std::uint32_t height, const char *name);
    ~window();

    template <typename T, class... Args> T *add_render_system(Args &&...args)
    {
        static_assert(std::is_base_of<render_system2D, T>::value || std::is_base_of<render_system3D, T>::value,
                      "Type must inherit from render_system2D or render_system3D!");

        auto system = make_scope<T>(std::forward<Args>(args)...);
        T *ref = system.get();

        system->init(m_device, m_renderer->swap_chain().render_pass());
        if constexpr (std::is_base_of<render_system2D, T>::value)
            m_render_systems2D.push_back(std::move(system));
        else
            m_render_systems3D.push_back(std::move(system));
        return ref;
    }

    std::uint32_t width() const;
    std::uint32_t height() const;

    VkExtent2D extent() const;

    bool should_close() const;
    void create_surface(VkInstance instance, VkSurfaceKHR *surface) const;

    void poll_events();
    bool display();

    bool was_resized() const;
    void complete_resize();

    const device &gpu() const;

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    ref<const device> m_device;
    scope<renderer> m_renderer;
    std::vector<scope<render_system2D>> m_render_systems2D;
    std::vector<scope<render_system3D>> m_render_systems3D;

    bool m_frame_buffer_resized = false;

    void init();

    static void frame_buffer_resize_callback(GLFWwindow *gwindow, int width, int height);

    window(const window &) = delete;
    window &operator=(const window &) = delete;
};
} // namespace lynx

#endif