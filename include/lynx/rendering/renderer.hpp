#ifndef LYNX_RENDERER_HPP
#define LYNX_RENDERER_HPP

#include "lynx/internal/core.hpp"
#include "lynx/rendering/swap_chain.hpp"
#include <vulkan/vulkan.hpp>
#include <functional>

namespace lynx
{
class window;
class device;
class renderer : non_copyable
{
  public:
    renderer(const ref<const device> &dev, window &win);
    ~renderer();

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer, const glm::vec4 &clear_color);
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

    void immediate_submission(const std::function<void(VkCommandBuffer)> &submission) const;

    bool frame_in_progress() const;
    VkCommandBuffer current_command_buffer() const;
    std::uint32_t frame_index() const;
    const swap_chain &swap_chain() const;

  private:
    window &m_window;
    ref<const device> m_device;
    scope<lynx::swap_chain> m_swap_chain;
    std::array<VkCommandBuffer, swap_chain::MAX_FRAMES_IN_FLIGHT> m_command_buffers;

    std::uint32_t m_image_index;
    std::uint32_t m_frame_index = 0;
    bool m_frame_started = false;

    void create_command_buffers();
    void create_swap_chain();
    void free_command_buffers();
};
} // namespace lynx

#endif