#include "lynx/pch.hpp"
#include "lynx/swap_chain.hpp"
#include "lynx/exceptions.hpp"

namespace lynx
{

swap_chain::swap_chain(const device &dev, VkExtent2D extent, scope<swap_chain> old_swap_chain)
    : m_device(dev), m_old_swap_chain(std::move(old_swap_chain)), m_window_extent(extent)
{
    init();
    create_image_views();
    create_render_pass();
    create_depth_resources();
    create_frame_buffers();
    create_sync_objects();
    m_old_swap_chain = nullptr;
}

swap_chain::~swap_chain()
{
    for (VkImageView image_view : m_swap_chain_image_views)
        vkDestroyImageView(m_device.vulkan_device(), image_view, nullptr);
    m_swap_chain_image_views.clear();

    if (m_swap_chain)
    {
        vkDestroySwapchainKHR(m_device.vulkan_device(), m_swap_chain, nullptr);
        m_swap_chain = nullptr;
    }

    for (std::size_t i = 0; i < m_depth_images.size(); i++)
    {
        vkDestroyImageView(m_device.vulkan_device(), m_depth_image_views[i], nullptr);
        vkDestroyImage(m_device.vulkan_device(), m_depth_images[i], nullptr);
        vkFreeMemory(m_device.vulkan_device(), m_depth_image_memories[i], nullptr);
    }

    for (auto frame_buffer : m_swap_chain_frame_buffers)
        vkDestroyFramebuffer(m_device.vulkan_device(), frame_buffer, nullptr);

    vkDestroyRenderPass(m_device.vulkan_device(), m_render_pass, nullptr);

    // cleanup synchronization objects
    for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_device.vulkan_device(), m_render_finished_semaphores[i], nullptr);
        vkDestroySemaphore(m_device.vulkan_device(), m_image_available_semaphores[i], nullptr);
        vkDestroyFence(m_device.vulkan_device(), m_in_flight_fences[i], nullptr);
    }
}

VkResult swap_chain::acquire_next_image(std::uint32_t *image_index) const
{
    vkWaitForFences(m_device.vulkan_device(), 1, &m_in_flight_fences[m_current_frame], VK_TRUE,
                    std::numeric_limits<uint64_t>::max());

    VkResult result =
        vkAcquireNextImageKHR(m_device.vulkan_device(), m_swap_chain, std::numeric_limits<uint64_t>::max(),
                              m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE, image_index);

    return result;
}

VkResult swap_chain::submit_command_buffers(const VkCommandBuffer *buffers, std::uint32_t *image_index)
{
    if (m_images_in_flight[*image_index] != VK_NULL_HANDLE)
        vkWaitForFences(m_device.vulkan_device(), 1, &m_images_in_flight[*image_index], VK_TRUE, UINT64_MAX);

    m_images_in_flight[*image_index] = m_in_flight_fences[m_current_frame];

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::array<VkSemaphore, 1> wait_semaphores = {m_image_available_semaphores[m_current_frame]};
    std::array<VkPipelineStageFlags, 1> wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores.data();
    submit_info.pWaitDstStageMask = wait_stages.data();

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = buffers;

    std::array<VkSemaphore, 1> signal_semaphores = {m_render_finished_semaphores[m_current_frame]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores.data();

    vkResetFences(m_device.vulkan_device(), 1, &m_in_flight_fences[m_current_frame]);
    if (vkQueueSubmit(m_device.graphics_queue(), 1, &submit_info, m_in_flight_fences[m_current_frame]) != VK_SUCCESS)
        throw bad_init("Failed to submit draw command buffer!");

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores.data();

    std::array<VkSwapchainKHR, 1> swap_chains = {m_swap_chain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains.data();
    present_info.pImageIndices = image_index;

    VkResult result = vkQueuePresentKHR(m_device.present_queue(), &present_info);
    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void swap_chain::init()
{
    device::swap_chain_support_details swap_chain_support = m_device.swap_chain_support();

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
    VkExtent2D extent = choose_swap_extent(swap_chain_support.capabilities);

    std::uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.capabilities.maxImageCount)
        image_count = swap_chain_support.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_device.surface();

    createInfo.minImageCount = image_count;
    createInfo.imageFormat = surface_format.format;
    createInfo.imageColorSpace = surface_format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    device::queue_family_indices indices = m_device.find_physical_queue_families();
    std::array<std::uint32_t, 2> queue_family_indices = {indices.graphics_family, indices.present_family};

    if (indices.graphics_family != indices.present_family)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queue_family_indices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = present_mode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = m_old_swap_chain ? m_old_swap_chain->m_swap_chain : VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_device.vulkan_device(), &createInfo, nullptr, &m_swap_chain) != VK_SUCCESS)
        throw bad_init("Failed to create swap chain!");

    // We only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(m_device.vulkan_device(), m_swap_chain, &image_count, nullptr);
    m_swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_device.vulkan_device(), m_swap_chain, &image_count, m_swap_chain_images.data());

    m_swap_chain_image_format = surface_format.format;
    m_extent = extent;
}

void swap_chain::create_image_views()
{
    m_swap_chain_image_views.resize(m_swap_chain_images.size());
    for (std::size_t i = 0; i < m_swap_chain_images.size(); i++)
    {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = m_swap_chain_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = m_swap_chain_image_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device.vulkan_device(), &view_info, nullptr, &m_swap_chain_image_views[i]) !=
            VK_SUCCESS)
            throw bad_init("Failed to create texture image view!");
    }
}

void swap_chain::create_render_pass()
{
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = find_depth_format();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_swap_chain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {color_attachment, depth_attachment};
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device.vulkan_device(), &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
        throw bad_init("Failed to create render pass!");
}

void swap_chain::create_frame_buffers()
{
    m_swap_chain_frame_buffers.resize(m_swap_chain_images.size());
    for (std::size_t i = 0; i < m_swap_chain_images.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {m_swap_chain_image_views[i], m_depth_image_views[i]};

        VkFramebufferCreateInfo frame_buffer_info{};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.renderPass = m_render_pass;
        frame_buffer_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
        frame_buffer_info.pAttachments = attachments.data();
        frame_buffer_info.width = m_extent.width;
        frame_buffer_info.height = m_extent.height;
        frame_buffer_info.layers = 1;

        if (vkCreateFramebuffer(m_device.vulkan_device(), &frame_buffer_info, nullptr,
                                &m_swap_chain_frame_buffers[i]) != VK_SUCCESS)
            throw bad_init("Failed to create frame_buffer!");
    }
}

void swap_chain::create_depth_resources()
{
    VkFormat depth_format = find_depth_format();

    m_depth_images.resize(m_swap_chain_images.size());
    m_depth_image_memories.resize(m_swap_chain_images.size());
    m_depth_image_views.resize(m_swap_chain_images.size());

    for (std::size_t i = 0; i < m_depth_images.size(); i++)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = m_extent.width;
        image_info.extent.height = m_extent.height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = depth_format;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.flags = 0;

        m_device.create_image_with_info(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_images[i],
                                        m_depth_image_memories[i]);

        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = m_depth_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = depth_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device.vulkan_device(), &view_info, nullptr, &m_depth_image_views[i]) != VK_SUCCESS)
            throw bad_init("Failed to create texture image view!");
    }
}

void swap_chain::create_sync_objects()
{
    m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    m_images_in_flight.resize(m_swap_chain_images.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        if (vkCreateSemaphore(m_device.vulkan_device(), &semaphore_info, nullptr, &m_image_available_semaphores[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(m_device.vulkan_device(), &semaphore_info, nullptr, &m_render_finished_semaphores[i]) !=
                VK_SUCCESS ||
            vkCreateFence(m_device.vulkan_device(), &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
            throw bad_init("failed to create synchronization objects for a frame!");
}

VkSurfaceFormatKHR swap_chain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for (const auto &available_format : available_formats)
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return available_format;

    return available_formats[0];
}

VkPresentModeKHR swap_chain::choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes)
{
    for (const auto &available_present_mode : available_present_modes)
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            DBG_INFO("Present mode: Mailbox")
            return available_present_mode;
        }

    for (const auto &available_present_mode : available_present_modes)
        if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            DBG_INFO("Present mode: Immediate")
            return available_present_mode;
        }

    DBG_INFO("Present mode: V-Sync")
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swap_chain::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
        return capabilities.currentExtent;

    VkExtent2D actual_extent = m_window_extent;
    actual_extent.width =
        std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(capabilities.minImageExtent.height,
                                    std::min(capabilities.maxImageExtent.height, actual_extent.height));

    return actual_extent;
}

VkFormat swap_chain::find_depth_format() const
{
    return m_device.find_supported_format(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFramebuffer swap_chain::frame_buffer(const std::size_t index) const
{
    return m_swap_chain_frame_buffers[index];
}
VkRenderPass swap_chain::render_pass() const
{
    return m_render_pass;
}
VkImageView swap_chain::image_view(const std::size_t index) const
{
    return m_swap_chain_image_views[index];
}
std::size_t swap_chain::image_count() const
{
    return m_swap_chain_images.size();
}
VkFormat swap_chain::swap_chain_image_format() const
{
    return m_swap_chain_image_format;
}
VkExtent2D swap_chain::extent() const
{
    return m_extent;
}
std::uint32_t swap_chain::width() const
{
    return m_extent.width;
}
std::uint32_t swap_chain::height() const
{
    return m_extent.height;
}

float swap_chain::extent_aspect_ratio() const
{
    return (float)m_extent.width / (float)m_extent.height;
}

} // namespace lynx