#ifndef LYNX_RENDERER_HPP
#define LYNX_RENDERER_HPP

#include "lynx/rendering/swap_chain.hpp"
#include "lynx/drawing/color.hpp"
#include "kit/memory/ref.hpp"
#include "kit/memory/scope.hpp"

#include <vulkan/vulkan.hpp>
#include <functional>
#include <thread>

namespace lynx
{
class window;
class device;
class renderer : kit::non_copyable
{
  public:
    renderer(const kit::ref<const device> &dev, window &win);
    ~renderer();

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer, const color &clear_color);
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

    void immediate_submission(const std::function<void(VkCommandBuffer)> &submission) const;

    bool frame_in_progress() const;
    VkCommandBuffer current_command_buffer() const;
    std::uint32_t frame_index() const;
    const swap_chain &swap_chain() const;

#ifdef LYNX_MULTITHREADED
    void wait_for_queue_submission() const;
#endif

  private:
    window &m_window;
    kit::ref<const device> m_device;
    kit::scope<lynx::swap_chain> m_swap_chain;
    std::array<VkCommandBuffer, swap_chain::MAX_FRAMES_IN_FLIGHT> m_command_buffers;

    std::uint32_t m_image_index;
    std::uint32_t m_frame_index = 0;
    bool m_frame_started = false;

#ifdef LYNX_MULTITHREADED
    mutable std::thread m_end_frame_thread;
#endif

    void end_frame_implementation();
    void create_command_buffers();
    void create_swap_chain();
    void free_command_buffers();
};
} // namespace lynx

#endif