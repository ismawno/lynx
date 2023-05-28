#ifndef LYNX_WINDOW_HPP
#define LYNX_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>

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

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    void init();

    window(const window &) = delete;
    window &operator=(const window &) = delete;

    window(window &&) = default;
    window &operator=(window &&) = default;
};
} // namespace lynx

#endif