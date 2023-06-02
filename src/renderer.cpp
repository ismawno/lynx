#include "lynx/pch.hpp"
#include "lynx/renderer.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/window.hpp"
#include "lynx/device.hpp"

namespace lynx
{
renderer::renderer(const device &dev, window &win) : m_window(win), m_device(dev)
{
    create_swap_chain();
    create_command_buffers();
}

renderer::~renderer()
{
    free_command_buffers();
}

bool renderer::frame_in_progress() const
{
    return m_frame_started;
}
VkCommandBuffer renderer::current_command_buffer() const
{
    DBG_ASSERT_ERROR(m_frame_started, "Frame must have started to retrieve command buffer")
    return m_command_buffers[m_frame_index];
}

std::uint32_t renderer::frame_index() const
{
    DBG_ASSERT_ERROR(m_frame_started, "Frame must have started to retrieve frame index")
    return m_frame_index;
}

void renderer::create_swap_chain()
{
    VkExtent2D ext = m_window.extent();
    while (ext.width == 0 || ext.height == 0)
    {
        ext = m_window.extent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device.vulkan_device());
    m_swap_chain = make_scope<swap_chain>(m_device, ext, std::move(m_swap_chain));
    // create_pipeline(); // If render passes are not compatible
}

void renderer::create_command_buffers()
{
    m_command_buffers.resize(swap_chain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_device.command_pool();
    alloc_info.commandBufferCount = (std::uint32_t)m_command_buffers.size();

    if (vkAllocateCommandBuffers(m_device.vulkan_device(), &alloc_info, m_command_buffers.data()) != VK_SUCCESS)
        throw bad_init("Failed to create command buffers");
}

void renderer::free_command_buffers()
{
    vkFreeCommandBuffers(m_device.vulkan_device(), m_device.command_pool(), (std::uint32_t)m_command_buffers.size(),
                         m_command_buffers.data());
}

VkCommandBuffer renderer::begin_frame()
{
    DBG_ASSERT_ERROR(!m_frame_started, "Cannot begin a new frame when there is already one in progress")
    const VkResult result = m_swap_chain->acquire_next_image(&m_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        create_swap_chain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw draw_error("Failed to acquire swap chain image");

    m_frame_started = true;
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(m_command_buffers[m_frame_index], &begin_info) != VK_SUCCESS)
        throw bad_init("Failed to begin command buffer");
    return m_command_buffers[m_frame_index];
}
void renderer::end_frame()
{
    DBG_ASSERT_ERROR(m_frame_started, "Cannot end a frame when there is no frame in progress")
    if (vkEndCommandBuffer(m_command_buffers[m_frame_index]) != VK_SUCCESS)
        throw bad_deinit("Failed to end command buffer");

    const VkResult result = m_swap_chain->submit_command_buffers(&m_command_buffers[m_frame_index], &m_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.was_resized())
    {
        create_swap_chain();
        m_window.complete_resize();
    }
    else if (result != VK_SUCCESS)
        throw draw_error("Failed to submit command buffers");
    m_frame_started = false;
    m_frame_index = (m_frame_index + 1) % swap_chain::MAX_FRAMES_IN_FLIGHT;
}

void renderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer) const
{
    DBG_ASSERT_ERROR(m_frame_started, "Cannot begin render pass if a frame is not in progress")
    DBG_ASSERT_ERROR(m_command_buffers[m_frame_index] == command_buffer,
                     "Cannot begin render pass with a command buffer from another frame")

    VkRenderPassBeginInfo pass_info{};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = m_swap_chain->render_pass();
    pass_info.framebuffer = m_swap_chain->frame_buffer(m_image_index);
    pass_info.renderArea.offset = {0, 0};
    pass_info.renderArea.extent = m_swap_chain->extent();

    std::array<VkClearValue, 2> clear_values;
    clear_values[0].color = {{0.01f, 0.01f, 0.01f, 1.f}};
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
void renderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer) const
{
    DBG_ASSERT_ERROR(m_frame_started, "Cannot end render pass if a frame is not in progress")
    DBG_ASSERT_ERROR(m_command_buffers[m_frame_index] == command_buffer,
                     "Cannot end render pass with a command buffer from another frame")

    vkCmdEndRenderPass(m_command_buffers[m_frame_index]);
}
} // namespace lynx