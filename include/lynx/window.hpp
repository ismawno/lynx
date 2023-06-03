#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>

#include "lynx/core.hpp"

namespace lynx
{
class model2D;
class device;
class renderer;
class window
{
  public:
    window(std::uint32_t width, std::uint32_t height, const char *name);
    ~window();

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

    scope<const device> m_device;
    scope<model2D> m_model;
    scope<renderer> m_renderer;

    bool m_frame_buffer_resized = false;

    void init();

    void load_models();

    static void frame_buffer_resize_callback(GLFWwindow *gwindow, int width, int height);

    window(const window &) = delete;
    window &operator=(const window &) = delete;
};
} // namespace lynx

#endif