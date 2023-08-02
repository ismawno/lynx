#include "lynx/internal/pch.hpp"
#include "lynx/rendering/renderer.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/device.hpp"
#include "kit/profile/perf.hpp"

namespace lynx
{
renderer::renderer(const kit::ref<const device> &dev, window &win) : m_window(win), m_device(dev)
{
    create_swap_chain();
    create_command_buffers();
}

renderer::~renderer()
{
#ifdef LYNX_MULTITHREADED
    wait_for_queue_submission();
#endif
    free_command_buffers();
}

bool renderer::frame_in_progress() const
{
    return m_frame_started;
}
VkCommandBuffer renderer::current_command_buffer() const
{
    KIT_ASSERT_ERROR(m_frame_started, "Frame must have started to retrieve command buffer")
    return m_command_buffers[m_frame_index];
}

std::uint32_t renderer::frame_index() const
{
    KIT_ASSERT_ERROR(m_frame_started, "Frame must have started to retrieve frame index")
    return m_frame_index;
}

const swap_chain &renderer::swap_chain() const
{
    return *m_swap_chain;
}

void renderer::create_swap_chain()
{
    VkExtent2D ext = m_window.extent();
    while (ext.width == 0 || ext.height == 0)
    {
        ext = m_window.extent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device->vulkan_device());
    m_swap_chain = kit::make_scope<lynx::swap_chain>(m_device, ext, std::move(m_swap_chain));
    // create_pipeline(); // If render passes are not compatible
}

void renderer::create_command_buffers()
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_device->command_pool();
    alloc_info.commandBufferCount = (std::uint32_t)m_command_buffers.size();

    KIT_CHECK_RETURN_VALUE(vkAllocateCommandBuffers(m_device->vulkan_device(), &alloc_info, m_command_buffers.data()),
                           VK_SUCCESS, CRITICAL, "Failed to create command buffers")
}

void renderer::free_command_buffers()
{
    vkFreeCommandBuffers(m_device->vulkan_device(), m_device->command_pool(), (std::uint32_t)m_command_buffers.size(),
                         m_command_buffers.data());
}

VkCommandBuffer renderer::begin_frame()
{
    KIT_PERF_FUNCTION()
#ifdef LYNX_MULTITHREADED
    wait_for_queue_submission();
#endif

    KIT_ASSERT_ERROR(!m_frame_started, "Cannot begin a new frame when there is already one in progress")

    const VkResult result = m_swap_chain->acquire_next_image(&m_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        create_swap_chain();
        return nullptr;
    }

    KIT_ASSERT_CRITICAL(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image")
    m_frame_started = true;
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    KIT_CHECK_RETURN_VALUE(vkResetCommandBuffer(m_command_buffers[m_frame_index], 0), VK_SUCCESS, CRITICAL,
                           "Failed to reset command buffer")
    KIT_CHECK_RETURN_VALUE(vkBeginCommandBuffer(m_command_buffers[m_frame_index], &begin_info), VK_SUCCESS, CRITICAL,
                           "Failed to begin command buffer")
    return m_command_buffers[m_frame_index];
}
void renderer::end_frame()
{
    KIT_PERF_FUNCTION()
#ifdef LYNX_MULTITHREADED
    m_end_frame_thread = std::thread(&renderer::end_frame_implementation, this);
#else
    end_frame_implementation();
#endif
}

#ifdef LYNX_MULTITHREADED
void renderer::wait_for_queue_submission() const
{
    if (m_end_frame_thread.joinable())
        m_end_frame_thread.join();
}
#endif

void renderer::end_frame_implementation()
{
    KIT_ASSERT_ERROR(m_frame_started, "Cannot end a frame when there is no frame in progress")
    KIT_CHECK_RETURN_VALUE(vkEndCommandBuffer(m_command_buffers[m_frame_index]), VK_SUCCESS, CRITICAL,
                           "Failed to end command buffer")

    const VkResult result = m_swap_chain->submit_command_buffers(&m_command_buffers[m_frame_index], &m_image_index);

    const bool recreate_fixes_issue =
        result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.was_resized();
    if (recreate_fixes_issue)
    {
        create_swap_chain();
        m_window.complete_resize();
    }

    KIT_ASSERT_CRITICAL(recreate_fixes_issue || result == VK_SUCCESS, "Failed to submit command buffers")
    m_frame_started = false;
    m_frame_index = (m_frame_index + 1) % swap_chain::MAX_FRAMES_IN_FLIGHT;
}

void renderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer, const color &clear_color)
{
    KIT_ASSERT_ERROR(m_frame_started, "Cannot begin render pass if a frame is not in progress")
    KIT_ASSERT_ERROR(m_command_buffers[m_frame_index] == command_buffer,
                     "Cannot begin render pass with a command buffer from another frame")
    KIT_PERF_FUNCTION()

    VkRenderPassBeginInfo pass_info{};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = m_swap_chain->render_pass();
    pass_info.framebuffer = m_swap_chain->frame_buffer(m_image_index);
    pass_info.renderArea.offset = {0, 0};
    pass_info.renderArea.extent = m_swap_chain->extent();

    std::array<VkClearValue, 2> clear_values;
    clear_values[0].color = {
        {clear_color.normalized.r, clear_color.normalized.g, clear_color.normalized.b, clear_color.normalized.a}};
    clear_values[1].depthStencil = {1, 0};

    pass_info.clearValueCount = 2;
    pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(m_command_buffers[m_frame_index], &pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swap_chain->width();
    viewport.height = (float)m_swap_chain->height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = {m_swap_chain->width(), m_swap_chain->height()};

    vkCmdSetViewport(m_command_buffers[m_frame_index], 0, 1, &viewport);
    vkCmdSetScissor(m_command_buffers[m_frame_index], 0, 1, &scissor);
}
void renderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer)
{
    KIT_ASSERT_ERROR(m_frame_started, "Cannot end render pass if a frame is not in progress")
    KIT_ASSERT_ERROR(m_command_buffers[m_frame_index] == command_buffer,
                     "Cannot end render pass with a command buffer from another frame")
    KIT_PERF_FUNCTION()

    vkCmdEndRenderPass(m_command_buffers[m_frame_index]);
}

void renderer::immediate_submission(const std::function<void(VkCommandBuffer)> &submission) const
{
#ifdef LYNX_MULTITHREADED
    wait_for_queue_submission();
#endif
    const VkCommandBuffer command_buffer = m_device->begin_single_time_commands();
    submission(command_buffer);
    m_device->end_single_time_commands(command_buffer);
}
} // namespace lynx