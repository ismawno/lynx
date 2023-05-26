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

    bool should_close() const;

  private:
    std::uint32_t m_width, m_height;
    const char *m_name;
    GLFWwindow *m_window;

    void init_window();

    window(const window &we) = delete;
    window &operator=(const window &) = delete;
};
} // namespace lynx

#endif