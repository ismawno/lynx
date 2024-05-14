#pragma once

#include "lynx/rendering/swap_chain.hpp"
#include "lynx/drawing/color.hpp"
#include "lynx/internal/dimension.hpp"
#include "lynx/rendering/device.hpp"
#include "kit/memory/ptr/ref.hpp"
#include "kit/memory/ptr/scope.hpp"

#include <vulkan/vulkan.hpp>
#include <functional>

namespace lynx
{

template <Dimension Dim> class renderer : kit::non_copyable
{
  public:
    using window_t = window<Dim>;

    renderer(const kit::ref<const device> &dev, window_t &win);
    ~renderer();

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer, const color &clear_color);
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

    template <kit::Callable<VkCommandBuffer> F> void immediate_submission(F submission) const
    {
        const VkCommandBuffer command_buffer = m_device->begin_single_time_commands();
        submission(command_buffer);
        m_device->end_single_time_commands(command_buffer);
    }

    bool frame_in_progress() const;
    VkCommandBuffer current_command_buffer() const;
    std::uint32_t frame_index() const;
    const swap_chain &swap_chain() const;

  private:
    window_t &m_window;
    kit::ref<const device> m_device;
    kit::scope<lynx::swap_chain> m_swap_chain;
    std::array<VkCommandBuffer, swap_chain::MAX_FRAMES_IN_FLIGHT> m_command_buffers;

    std::uint32_t m_image_index;
    std::uint32_t m_frame_index = 0;
    bool m_frame_started = false;

    void create_command_buffers();
    void create_swap_chain();
    void free_command_buffers();
};

using renderer2D = renderer<dimension::two>;
using renderer3D = renderer<dimension::three>;
} // namespace lynx
