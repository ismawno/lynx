#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>

#include "lynx/core.hpp"

namespace lynx
{
class device;
class pipeline;
class model;
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

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    ref<const device> m_device;
    scope<pipeline> m_pipeline;
    scope<model> m_model;
    scope<renderer> m_renderer;

    VkPipelineLayout m_pipeline_layout;

    bool m_frame_buffer_resized = false;

    void init();

    void load_models();
    void create_pipeline_layout();
    void create_pipeline();

    static void frame_buffer_resize_callback(GLFWwindow *gwindow, int width, int height);

    window(const window &) = delete;
    window &operator=(const window &) = delete;
};
} // namespace lynx

#endif