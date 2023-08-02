#ifndef LYNX_SWAP_CHAIN_HPP
#define LYNX_SWAP_CHAIN_HPP

#include "kit/memory/ref.hpp"
#include "kit/memory/scope.hpp"

#include "lynx/rendering/device.hpp"
#include "kit/interface/non_copyable.hpp"
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace lynx
{

class swap_chain : kit::non_copyable
{
  public:
    static constexpr std::uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    swap_chain(const kit::ref<const device> &dev, VkExtent2D window_extentm, kit::scope<swap_chain> old_swap_chain);
    ~swap_chain();

    VkFramebuffer frame_buffer(std::size_t index) const;
    VkRenderPass render_pass() const;
    VkImageView image_view(std::size_t index) const;
    std::size_t image_count() const;
    VkFormat swap_chain_image_format() const;
    VkExtent2D extent() const;
    std::uint32_t width() const;
    std::uint32_t height() const;
    float extent_aspect_ratio() const;

    VkFormat find_depth_format() const;

    VkResult acquire_next_image(std::uint32_t *image_index) const;
    VkResult submit_command_buffers(const VkCommandBuffer *buffers, const std::uint32_t *image_index);

  private:
    void init();
    void create_image_views();
    void create_depth_resources();
    void create_render_pass();
    void create_frame_buffers();
    void create_sync_objects();

    bool compare_swap_formats(const swap_chain &swpc) const;

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat m_swap_chain_image_format;
    VkFormat m_swap_chain_depth_format;
    VkExtent2D m_extent;

    std::vector<VkFramebuffer> m_swap_chain_frame_buffers;
    VkRenderPass m_render_pass;

    std::vector<VkImage> m_depth_images;
    std::vector<VkDeviceMemory> m_depth_image_memories;
    std::vector<VkImageView> m_depth_image_views;
    std::vector<VkImage> m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;

    kit::ref<const device> m_device;
    kit::scope<swap_chain> m_old_swap_chain;

    VkExtent2D m_window_extent;

    VkSwapchainKHR m_swap_chain;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_image_available_semaphores;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_render_finished_semaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;
    std::size_t m_current_frame = 0;
};

} // namespace lynx

#endif
