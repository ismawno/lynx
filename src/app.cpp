#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/exceptions.hpp"

namespace lynx
{
struct push_constant_data // Needs to be restructured
{
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

app::app(const std::uint32_t width, const std::uint32_t height, const char *name) : m_window(width, height, name)
{
    load_models();
    create_pipeline_layout();
    create_swap_chain();
    create_command_buffers();
}

app::~app()
{
    vkDestroyPipelineLayout(m_device.vulkan_device(), m_pipeline_layout, nullptr);
}

void app::run()
{
    while (!m_window.should_close())
    {
        glfwPollEvents();
        draw_frame();
    }
    vkDeviceWaitIdle(m_device.vulkan_device());
}

void app::load_models()
{
    const std::vector<model::vertex> vertices = {
        {{0.f, -0.5f}, {1.f, 0.f, 0.f}}, {{0.5f, 0.5f}, {0.f, 1.f, 0.f}}, {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    m_model = make_scope<model>(m_device, vertices);
}

void app::create_pipeline_layout()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(push_constant_data);

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pSetLayouts = nullptr;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
    if (vkCreatePipelineLayout(m_device.vulkan_device(), &layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        throw bad_init("Failed to create pipeline layout");
}

void app::create_pipeline()
{
    DBG_ASSERT_ERROR(m_swap_chain, "Cannot create pipeline before swap chain!")
    DBG_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!")
    pipeline::config_info pip_config{};
    pipeline::config_info::default_config(pip_config);
    pip_config.render_pass = m_swap_chain->render_pass();
    pip_config.pipeline_layout = m_pipeline_layout;
    m_pipeline = make_scope<pipeline>(m_device, LYNX_VERTEX_SHADER_PATH, LYNX_FRAGMENT_SHADER_PATH, pip_config);
}

void app::create_command_buffers()
{
    m_command_buffers.resize(m_swap_chain->image_count());

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_device.command_pool();
    alloc_info.commandBufferCount = (std::uint32_t)m_command_buffers.size();

    if (vkAllocateCommandBuffers(m_device.vulkan_device(), &alloc_info, m_command_buffers.data()) != VK_SUCCESS)
        throw bad_init("Failed to create command buffers");
}

void app::free_command_buffers()
{
    vkFreeCommandBuffers(m_device.vulkan_device(), m_device.command_pool(), (std::uint32_t)m_command_buffers.size(),
                         m_command_buffers.data());
}

void app::record_command_buffer(const std::size_t image_index)
{
    static std::uint32_t frame = 0;
    frame = (frame + 1) % 1000;

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(m_command_buffers[image_index], &begin_info) != VK_SUCCESS)
        throw bad_init("Failed to begin command buffer");

    VkRenderPassBeginInfo pass_info{};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = m_swap_chain->render_pass();
    pass_info.framebuffer = m_swap_chain->frame_buffer(image_index);
    pass_info.renderArea.offset = {0, 0};
    pass_info.renderArea.extent = m_swap_chain->extent();

    std::array<VkClearValue, 2> clear_values;
    clear_values[0].color = {{0.01f, 0.01f, 0.01f, 1.f}};
    clear_values[1].depthStencil = {1, 0};

    pass_info.clearValueCount = 2;
    pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(m_command_buffers[image_index], &pass_info, VK_SUBPASS_CONTENTS_INLINE);

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

    vkCmdSetViewport(m_command_buffers[image_index], 0, 1, &viewport);
    vkCmdSetScissor(m_command_buffers[image_index], 0, 1, &scissor);

    m_pipeline->bind(m_command_buffers[image_index]);
    m_model->bind(m_command_buffers[image_index]);

    for (std::size_t i = 0; i < 4; i++)
    {
        const push_constant_data push_data{{-0.5f + frame * 0.0005f, -0.4f + i * 0.25f}, {0.f, 0.f, 0.2f + i * 0.2f}};
        vkCmdPushConstants(m_command_buffers[image_index], m_pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push_constant_data),
                           &push_data);
        m_model->draw(m_command_buffers[image_index]);
    }

    vkCmdEndRenderPass(m_command_buffers[image_index]);
    if (vkEndCommandBuffer(m_command_buffers[image_index]) != VK_SUCCESS)
        throw bad_deinit("Failed to end command buffer");
}

void app::create_swap_chain()
{
    VkExtent2D extent = m_window.extent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = m_window.extent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device.vulkan_device());
    const bool command_buffer_check_required = m_swap_chain != nullptr;
    m_swap_chain = make_scope<swap_chain>(m_device, extent, std::move(m_swap_chain));
    if (command_buffer_check_required && m_swap_chain->image_count() != m_command_buffers.size())
    {
        free_command_buffers();
        create_command_buffers();
    }
    create_pipeline(); // If render passes are not compatible
}

void app::draw_frame()
{
    std::uint32_t image_index;
    VkResult result = m_swap_chain->acquire_next_image(&image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        create_swap_chain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw draw_error("Failed to acquire swap chain image");

    record_command_buffer(image_index);
    result = m_swap_chain->submit_command_buffers(&m_command_buffers[image_index], &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.was_resized())
    {
        create_swap_chain();
        m_window.resize_complete();
        return;
    }
    if (result != VK_SUCCESS)
        throw draw_error("Failed to submit command buffers");
}
} // namespace lynx