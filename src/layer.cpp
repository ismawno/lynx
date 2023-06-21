#include "lynx/pch.hpp"
#include "lynx/layer.hpp"
#include "lynx/app.hpp"

#include <imgui.h>

namespace lynx
{
layer::layer(const char *name) : m_name(name)
{
}

imgui_layer::imgui_layer(const char *name) : layer(name)
{
}

const char *layer::name() const
{
    return m_name;
}

void imgui_layer::on_attach()
{
    const app *parent_app = parent();
    constexpr std::uint32_t pool_size = 1000;
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, pool_size},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, pool_size},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, pool_size},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, pool_size},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, pool_size},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, pool_size},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, pool_size},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, pool_size},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, pool_size},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, pool_size},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, pool_size}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    DBG_CHECK_RETURN_VALUE(
        vkCreateDescriptorPool(parent_app->window()->device().vulkan_device(), &pool_info, nullptr, &m_imgui_pool),
        VK_SUCCESS, CRITICAL, "Failed to create descriptor pool")

    m_imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imgui_context);

    IMGUI_CHECKVERSION();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForVulkan(parent_app->window()->glfw_window(), true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = parent_app->window()->device().vulkan_instance();
    init_info.PhysicalDevice = parent_app->window()->device().vulkan_physical_device();
    init_info.Device = parent_app->window()->device().vulkan_device();
    init_info.Queue = parent_app->window()->device().graphics_queue();
    init_info.DescriptorPool = m_imgui_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, parent_app->window()->renderer().swap_chain().render_pass());

    parent_app->window()->renderer().immediate_submission(
        [](const VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

    vkDeviceWaitIdle(parent_app->window()->device().vulkan_device());
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void imgui_layer::on_update(const float ts)
{
    const app *parent_app = parent();
    ImGui::SetCurrentContext(m_imgui_context);
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGui::PushID(this);
    on_imgui_render();
    ImGui::PopID();
    ImGui::Render();
}

void imgui_layer::on_detach()
{
    const app *parent_app = parent();
    ImGui::SetCurrentContext(m_imgui_context);
    vkDeviceWaitIdle(parent_app->window()->device().vulkan_device());
    vkDestroyDescriptorPool(parent_app->window()->device().vulkan_device(), m_imgui_pool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(m_imgui_context);
}

void imgui_layer::on_command_submission(const VkCommandBuffer command_buffer)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(current_context);
    }
}
} // namespace lynx