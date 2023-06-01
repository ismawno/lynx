#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>

#include "lynx/core.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/window.hpp"
#include "lynx/device.hpp"
#include "lynx/swap_chain.hpp"
#include "lynx/model.hpp"

namespace lynx
{
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
    void display();

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;
    ref<const device> m_device = nullptr;
    scope<swap_chain> m_swap_chain = nullptr;
    scope<pipeline> m_pipeline = nullptr;
    scope<model> m_model = nullptr;

    bool m_frame_buffer_resized = false;

    void init();

    VkPipelineLayout m_pipeline_layout;
    std::vector<VkCommandBuffer> m_command_buffers;

    void load_models();
    void create_pipeline_layout();
    void create_swap_chain();
    void create_pipeline();
    void create_command_buffers();
    void record_command_buffer(std::size_t image_index);

    void free_command_buffers();

    static void frame_buffer_resize_callback(GLFWwindow *gwindow, int width, int height);

    window(const window &) = delete;
    window &operator=(const window &) = delete;
};
} // namespace lynx

#endif