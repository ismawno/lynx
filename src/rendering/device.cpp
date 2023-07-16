#include "lynx/internal/pch.hpp"
#include "lynx/app/window.hpp"
#include "lynx/rendering/device.hpp"
#include "kit/container/stack_vector.hpp"

namespace lynx
{
#ifdef DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                     VkDebugUtilsMessageTypeFlagsEXT,
                                                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *)
{
    KIT_ERROR("{0}", pCallbackData->pMessage)
    return VK_FALSE;
}

VkResult create_debug_utils_messenger_EXT(const VkInstance m_instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func)
        return func(m_instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroy_debug_utils_messenger_EXT(VkInstance m_instance, VkDebugUtilsMessengerEXT m_debug_messenger,
                                       const VkAllocationCallbacks *pAllocator)
{
    auto func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
        func(m_instance, m_debug_messenger, pAllocator);
}
#endif

device::device(const window &win)
{
    create_instance();
#ifdef DEBUG
    setup_debug_messenger();
#endif
    win.create_surface(m_instance, &m_surface);
    pick_physical_device();
    create_logical_device();
    create_command_pool();
}

device::~device()
{
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    vkDestroyDevice(m_device, nullptr);

#ifdef DEBUG
    destroy_debug_utils_messenger_EXT(m_instance, m_debug_messenger, nullptr);
#endif

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

bool device::queue_family_indices::is_complete() const
{
    return graphics_family_has_value && present_family_has_value;
}

void device::create_instance()
{
    KIT_ASSERT_ERROR(check_validation_layer_support(), "Validation layers requested, but not available!")

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "LittleVulkanEngine App";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    const std::vector<const char *> extensions = required_extensions();
    create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
#ifdef __arm64__
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

#ifdef DEBUG
    create_info.enabledLayerCount = static_cast<std::uint32_t>(s_validation_layers.size());
    create_info.ppEnabledLayerNames = s_validation_layers.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    populate_debug_messenger_create_info(debug_create_info);
    create_info.pNext = &debug_create_info;
#else
    create_info.enabledLayerCount = 0;
    create_info.pNext = nullptr;
#endif

    KIT_CHECK_RETURN_VALUE(vkCreateInstance(&create_info, nullptr, &m_instance), VK_SUCCESS, CRITICAL,
                           "Failed to create vulkan instance")
#ifdef DEBUG
    has_gflw_required_instance_extensions();
#endif
}

void device::pick_physical_device()
{
    std::uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    KIT_ASSERT_CRITICAL(device_count != 0, "Failed to find GPUs with Vulkan support")

    KIT_INFO("Device count: {0}", device_count)
    kit::stack_vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

    for (const auto &device : devices)
        if (is_device_suitable(device))
        {
            m_physical_device = device;
            break;
        }

    KIT_ASSERT_CRITICAL(m_physical_device != VK_NULL_HANDLE, "Failed to find a suitable GPU")

    vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
    KIT_INFO("Physical device: {0}", m_properties.deviceName)
}

void device::create_logical_device()
{
    const queue_family_indices indices = find_queue_families(m_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    const std::unordered_set<std::uint32_t> unique_queue_family = {indices.graphics_family, indices.present_family};

    const float queue_priority = 1.0f;
    for (std::uint32_t queue_family : unique_queue_family)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount = (std::uint32_t)queue_create_infos.size();
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = (std::uint32_t)s_device_extensions.size();
    create_info.ppEnabledExtensionNames = s_device_extensions.data();

    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
#ifdef DEBUG
    create_info.enabledLayerCount = (std::uint32_t)s_validation_layers.size();
    create_info.ppEnabledLayerNames = s_validation_layers.data();
#else
    create_info.enabledLayerCount = 0;
#endif

    KIT_CHECK_RETURN_VALUE(vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device), VK_SUCCESS, CRITICAL,
                           "Failed to create logical device")

    vkGetDeviceQueue(m_device, indices.graphics_family, 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, indices.present_family, 0, &m_present_queue);
}

void device::create_command_pool()
{
    const queue_family_indices indices = find_physical_queue_families();

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = indices.graphics_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    KIT_CHECK_RETURN_VALUE(vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool), VK_SUCCESS, CRITICAL,
                           "Failed to create command pool")
}

bool device::is_device_suitable(const VkPhysicalDevice device) const
{
    const queue_family_indices indices = find_queue_families(device);

    const bool extensions_supported = check_device_extension_support(device);

    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
        swap_chain_support_details swap_chain_support = query_swap_chain_support(device);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    return indices.is_complete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
}

#ifdef DEBUG
void device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info) const
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = nullptr; // Optional
}
#endif

#ifdef DEBUG
void device::setup_debug_messenger()
{
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);
    KIT_CHECK_RETURN_VALUE(create_debug_utils_messenger_EXT(m_instance, &create_info, nullptr, &m_debug_messenger),
                           VK_SUCCESS, CRITICAL, "Failed to set up debug messenger")
}
#endif

#ifdef DEBUG
bool device::check_validation_layer_support() const
{
    std::uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    kit::stack_vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char *layer_name : s_validation_layers)
    {
        bool layer_found = false;

        for (const auto &layer_props : available_layers)
            if (strcmp(layer_name, layer_props.layerName) == 0)
            {
                layer_found = true;
                break;
            }

        if (!layer_found)
            return false;
    }

    return true;
}
#endif

std::vector<const char *> device::required_extensions() const
{
    std::uint32_t glfw_extension_count = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

#ifdef DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
#ifdef __arm64__
    extensions.insert(extensions.end(), {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                                         "VK_KHR_get_physical_device_properties2"});
#endif

    return extensions;
}

#ifdef DEBUG
void device::has_gflw_required_instance_extensions() const
{
    std::uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    kit::stack_vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    KIT_INFO("Available extensions:")
    std::unordered_set<std::string> available;
    for (const auto &extension : extensions)
    {
        KIT_INFO("\t{0}", extension.extensionName)
        available.insert(extension.extensionName);
    }

    KIT_INFO("Required extensions:")
    const std::vector<const char *> req_extensions = required_extensions();
    for (const auto &required : req_extensions)
    {
        KIT_INFO("\t{0}", required)
        KIT_ASSERT_CRITICAL(available.find(required) != available.end(), "Missing required glfw extension")
    }
}
#endif

bool device::check_device_extension_support(VkPhysicalDevice device) const
{
    std::uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    kit::stack_vector<VkExtensionProperties> availableExtensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, availableExtensions.data());

    std::unordered_set<std::string> req_extensions(s_device_extensions.begin(), s_device_extensions.end());

    for (const auto &extension : availableExtensions)
        req_extensions.erase(extension.extensionName);

    return req_extensions.empty();
}

device::queue_family_indices device::find_queue_families(VkPhysicalDevice device) const
{
    queue_family_indices indices;

    std::uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    kit::stack_vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (std::uint32_t i = 0; i < queue_families.size(); i++)
    {
        if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
            indices.graphics_family_has_value = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (queue_families[i].queueCount > 0 && presentSupport)
        {
            indices.present_family = i;
            indices.present_family_has_value = true;
        }
        if (indices.is_complete())
            break;
    }

    return indices;
}

device::swap_chain_support_details device::query_swap_chain_support(VkPhysicalDevice device) const
{
    swap_chain_support_details details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    std::uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, details.formats.data());
    }

    std::uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.present_modes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.present_modes.data());
    }
    return details;
}

VkFormat device::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                       VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

        if ((tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) ||
            (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features))
            return format;
    }
    KIT_CRITICAL("Failed to find supported format")
    return VK_FORMAT_MAX_ENUM;
}

std::uint32_t device::find_memory_type(std::uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);
    for (std::uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        if ((typeFilter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    KIT_CRITICAL("Failed to find suitable memory type");
    return (std::uint32_t)-1;
}

void device::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                           VkBuffer &buffer, VkDeviceMemory &buffer_memory) const
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    KIT_CHECK_RETURN_VALUE(vkCreateBuffer(m_device, &buffer_info, nullptr, &buffer), VK_SUCCESS, CRITICAL,
                           "Failed to create buffer")

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(m_device, buffer, &mem_reqs);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_reqs.memoryTypeBits, properties);

    KIT_CHECK_RETURN_VALUE(vkAllocateMemory(m_device, &alloc_info, nullptr, &buffer_memory), VK_SUCCESS, CRITICAL,
                           "Failed to allocate buffer memory")
    vkBindBufferMemory(m_device, buffer, buffer_memory, 0);
}

VkCommandBuffer device::begin_single_time_commands() const
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    KIT_CHECK_RETURN_VALUE(vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer), VK_SUCCESS, CRITICAL,
                           "Failed to allocate command buffer")

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    KIT_CHECK_RETURN_VALUE(vkBeginCommandBuffer(command_buffer, &begin_info), VK_SUCCESS, CRITICAL,
                           "Failed to begin command buffer")
    return command_buffer;
}

void device::end_single_time_commands(VkCommandBuffer command_buffer) const
{
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphics_queue);

    vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
}

void device::copy_buffer(VkBuffer dst_buffer, VkBuffer src_buffer, VkDeviceSize size) const
{
    VkCommandBuffer command_buffer = begin_single_time_commands();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    end_single_time_commands(command_buffer);
}

void device::copy_buffer_to_image(VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height,
                                  std::uint32_t layer_count) const
{
    VkCommandBuffer command_buffer = begin_single_time_commands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layer_count;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    end_single_time_commands(command_buffer);
}

void device::create_image_with_info(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties,
                                    VkImage &image, VkDeviceMemory &image_memory) const
{
    KIT_CHECK_RETURN_VALUE(vkCreateImage(m_device, &image_info, nullptr, &image), VK_SUCCESS, CRITICAL,
                           "Failed to create image!")

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(m_device, image, &mem_reqs);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_reqs.memoryTypeBits, properties);

    KIT_CHECK_RETURN_VALUE(vkAllocateMemory(m_device, &alloc_info, nullptr, &image_memory), VK_SUCCESS, CRITICAL,
                           "Failed to allocate image memory")
    KIT_CHECK_RETURN_VALUE(vkBindImageMemory(m_device, image, image_memory, 0), VK_SUCCESS, CRITICAL,
                           "Failed to bind image memory")
}

VkCommandPool device::command_pool() const
{
    return m_command_pool;
}
VkDevice device::vulkan_device() const
{
    return m_device;
}
VkPhysicalDevice device::vulkan_physical_device() const
{
    return m_physical_device;
}
VkInstance device::vulkan_instance() const
{
    return m_instance;
}
VkSurfaceKHR device::surface() const
{
    return m_surface;
}
VkQueue device::graphics_queue() const
{
    return m_graphics_queue;
}
VkQueue device::present_queue() const
{
    return m_present_queue;
}

VkPhysicalDeviceProperties device::properties() const
{
    return m_properties;
}

device::swap_chain_support_details device::swap_chain_support() const
{
    return query_swap_chain_support(m_physical_device);
}

device::queue_family_indices device::find_physical_queue_families() const
{
    return find_queue_families(m_physical_device);
}

} // namespace lynx