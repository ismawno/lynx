#ifndef LYNX_RENDERER_HPP
#define LYNX_RENDERER_HPP

#include "lynx/core.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class window;
class device;
class swap_chain;
class renderer
{
  public:
    renderer(const device &dev, window &win);
    ~renderer();

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer) const;
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer) const;

    bool frame_in_progress() const;
    VkCommandBuffer current_command_buffer() const;
    std::uint32_t frame_index() const;
    const swap_chain &swap_chain() const;

  private:
    window &m_window;
    const device &m_device;
    scope<lynx::swap_chain> m_swap_chain;
    std::vector<VkCommandBuffer> m_command_buffers;

    std::uint32_t m_image_index;
    std::uint32_t m_frame_index = 0;
    bool m_frame_started = false;

    void create_command_buffers();
    void create_swap_chain();
    void free_command_buffers();

    renderer(const renderer &) = delete;
    renderer &operator=(const renderer &) = delete;
};
} // namespace lynx

#endif