#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/camera.hpp"

namespace lynx
{
app::app(window &win) : m_window(win)
{
}

app::~app()
{
    DBG_ASSERT_ERROR(m_terminated || !m_started,
                     "Application being destroyed has not been terminated properly with shutdown()")
}

void app::run()
{
    start();
    while (next_frame())
        ;
    shutdown();
}

void app::start()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot call start on a terminated app")
    DBG_ASSERT_ERROR(!m_started, "Cannot call start on a started app")
    init_imgui();
    on_start();
    m_started = true;
}

bool app::next_frame()
{
    DBG_ASSERT_ERROR(!m_terminated, "Cannot fetch next frame on a terminated app")
    DBG_ASSERT_ERROR(m_started, "App must be started first by calling start() before fetching the next frame")
    if (m_window.closed())
        return false;
    m_window.poll_events();

    static float delta_time = 0.f;
    const auto start = std::chrono::high_resolution_clock::now();

    m_window.clear();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    on_update(delta_time);
    ImGui::ShowDemoWindow();

    if (m_window.closed())
        return false;

    ImGui::Render();
    m_window.display({[](const VkCommandBuffer cmd) { ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd); }});

    const auto end = std::chrono::high_resolution_clock::now();
    delta_time = std::chrono::duration<float, std::chrono::seconds::period>(end - start).count();
    return !m_window.closed();
}

void app::shutdown()
{
    DBG_ASSERT_ERROR(m_started, "Cannot terminate an app that has not been started")
    DBG_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")
    m_window.clear();
    on_shutdown();
    vkDestroyDescriptorPool(m_window.m_device->vulkan_device(), m_imgui_pool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    m_terminated = true;
}

void app::init_imgui()
{
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
        vkCreateDescriptorPool(m_window.m_device->vulkan_device(), &pool_info, nullptr, &m_imgui_pool), VK_SUCCESS,
        CRITICAL, "Failed to create descriptor pool")

    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForVulkan(m_window.glfw_window(), true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = m_window.m_device->vulkan_instance();
    init_info.PhysicalDevice = m_window.m_device->vulkan_physical_device();
    init_info.Device = m_window.m_device->vulkan_device();
    init_info.Queue = m_window.m_device->graphics_queue();
    init_info.DescriptorPool = m_imgui_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, m_window.m_renderer->swap_chain().render_pass());

    m_window.m_renderer->immediate_submission(
        [](const VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

    vkDeviceWaitIdle(m_window.m_device->vulkan_device());
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

app2D::app2D(const std::uint32_t width, const std::uint32_t height, const char *name)
    : app(m_window), m_window(width, height, name)
{
}

app3D::app3D(const std::uint32_t width, const std::uint32_t height, const char *name)
    : app(m_window), m_window(width, height, name)
{
}

} // namespace lynx