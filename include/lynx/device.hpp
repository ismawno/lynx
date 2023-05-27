#pragma once

#include "lynx/window.hpp"
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace lynx
{

class device
{
  private:
    struct swap_chain_support_details
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct queue_family_indices
    {
        std::uint32_t graphics_family;
        std::uint32_t present_family;
        bool graphics_family_has_value = false;
        bool present_family_has_value = false;
        bool is_complete() const;
    };

  public:
    explicit device(window &win);
    ~device();

    VkCommandPool command_pool() const;
    VkDevice vulkan_device() const;
    VkSurfaceKHR surface() const;
    VkQueue graphics_queue() const;
    VkQueue present_queue() const;

    swap_chain_support_details swap_chain_support() const;
    std::uint32_t find_memory_type(std::uint32_t type_filter, VkMemoryPropertyFlags properties) const;
    queue_family_indices find_physical_queue_families() const;
    VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                   VkFormatFeatureFlags features) const;

    // Buffer Helper Functions
    void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                       VkDeviceMemory &buffer_memory) const;

    VkCommandBuffer begin_single_time_commands() const;
    void end_single_time_commands(VkCommandBuffer command_buffer) const;
    void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) const;

    void copy_buffer_to_image(VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height,
                              std::uint32_t layer_count) const;

    void create_image_with_info(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image,
                                VkDeviceMemory &image_memory) const;

    VkPhysicalDeviceProperties m_properties;

  private:
    void create_instance();
#ifdef DEBUG
    void setup_debug_messenger();
#endif
    void create_surface();
    void pick_physical_device();
    void create_logical_device();
    void create_command_pool();

    bool is_device_suitable(VkPhysicalDevice device) const;
    std::vector<const char *> required_extensions() const;
    bool check_validation_layer_support() const;
    queue_family_indices find_queue_families(VkPhysicalDevice device) const;
    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo) const;
    void has_gflw_required_instance_extensions() const;
    bool check_device_extension_support(VkPhysicalDevice device) const;
    swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device) const;

    window &m_window;

    VkInstance m_instance;
#ifdef DEBUG
    VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkCommandPool m_command_pool;

    VkDevice m_device;
    VkSurfaceKHR m_surface;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    static inline const std::vector<const char *> s_validation_layers = {"VK_LAYER_KHRONOS_validation"};
    static inline const std::vector<const char *> s_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    device(const device &) = delete;
    void operator=(const device &) = delete;

    device(device &&) = delete;
    device &operator=(device &&) = delete;
};

} // namespace lynx