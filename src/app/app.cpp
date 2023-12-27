#include "lynx/internal/pch.hpp"
#include "lynx/app/app.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/serialization/serialization.hpp"
#include "kit/profile/perf.hpp"

namespace lynx
{

template <typename Dim> app<Dim>::~app()
{
    if (!m_terminated)
        shutdown();
}

template <typename Dim> void app<Dim>::run()
{
    start();
    while (next_frame())
        ;
    shutdown();
}

template <typename Dim> void app<Dim>::start()
{
    KIT_ASSERT_ERROR(!m_terminated, "Cannot call start on a terminated app")
    KIT_ASSERT_ERROR(!m_started, "Cannot call start on a started app")
    m_started = true;
    context_t::set(m_window.get());
#ifdef LYNX_ENABLE_IMGUI
    imgui_init();
#endif
    on_start();
    for (const auto &ly : m_layers)
        if (ly->enabled)
            ly->on_start();
    on_late_start();
}

template <typename Dim> bool app<Dim>::next_frame()
{
    KIT_ASSERT_ERROR(!m_terminated, "Cannot fetch next frame on a terminated app")
    KIT_ASSERT_ERROR(m_started, "App must be started first by calling start() before fetching the next frame")
    KIT_PERF_SCOPE("LYNX:Frame")

    if (m_min_frame_time > m_frame_time)
    {
        kit::time::sleep(m_min_frame_time - m_frame_time);
        m_frame_time = m_min_frame_time;
    }
    const kit::clock frame_clock;
    m_ongoing_frame = true;

    context_t::set(m_window.get());
    input_t::poll_events();
    if (m_window->closed())
    {
        m_ongoing_frame = false;
        return false;
    }

    while (const event ev = m_window->poll_event())
        if (!on_event(ev))
        {
            for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
                if ((*it)->enabled && (*it)->on_event(ev))
                    break;
            on_late_event(ev);
        }

    const float delta_time = m_frame_time.as<kit::time::seconds, float>();

    {
        KIT_PERF_SCOPE("LYNX:On-update") // MARK PERF SCOPES WITH LYNX/PPX-APP WHATEVER
        const kit::clock update_clock;

        on_update(delta_time);
        for (const auto &ly : m_layers)
            if (ly->enabled)
                ly->on_update(delta_time);
        on_late_update(delta_time);
        m_update_time = update_clock.elapsed();
    }

#ifdef LYNX_ENABLE_IMGUI
    imgui_begin_render();
#endif

    {
        KIT_PERF_SCOPE("LYNX:On-render")
        const kit::clock render_clock;

        on_render(delta_time);
        for (const auto &ly : m_layers)
            if (ly->enabled)
                ly->on_render(delta_time);
        on_late_render(delta_time);
        m_render_time = render_clock.elapsed();
    }

#ifdef LYNX_ENABLE_IMGUI
    imgui_end_render();
#endif

    const auto submission = [this](const VkCommandBuffer cmd) {
#ifdef LYNX_ENABLE_IMGUI
        imgui_submit_command(cmd);
#endif
        for (const auto &ly : m_layers)
            if (ly->enabled)
                ly->on_command_submission(cmd);
    };
    KIT_CHECK_RETURN_VALUE(m_window->display(submission), true, CRITICAL,
                           "Display failed to get command buffer for new frame")

    m_ongoing_frame = false;

    m_frame_time = frame_clock.elapsed();
    return !m_window->closed() && !m_to_finish_next_frame;
}

template <typename Dim> void app<Dim>::shutdown()
{
    context_t::set(m_window.get());
    if (m_ongoing_frame)
    {
        m_to_finish_next_frame = true;
        return;
    }
    KIT_ASSERT_ERROR(!m_terminated, "Cannot terminate an already terminated app")
    m_window->wait_for_device();

    on_shutdown();
    for (const auto &ly : m_layers)
        if (ly->enabled)
            ly->on_shutdown();
    on_late_shutdown();

#ifdef LYNX_ENABLE_IMGUI
    imgui_shutdown();
#endif

    m_terminated = true;
}

template <typename Dim> const window<Dim> *app<Dim>::window() const
{
    return m_window.get();
}

template <typename Dim> window<Dim> *app<Dim>::window()
{
    return m_window.get();
}

template <typename Dim> kit::time app<Dim>::frame_time() const
{
    return m_frame_time;
}
template <typename Dim> kit::time app<Dim>::update_time() const
{
    return m_update_time;
}
template <typename Dim> kit::time app<Dim>::render_time() const
{
    return m_render_time;
}

template <typename Dim> const std::vector<kit::scope<layer<Dim>>> &app<Dim>::layers() const
{
    return m_layers;
}

template <typename Dim> std::uint32_t app<Dim>::framerate_cap() const
{
    const bool capped = m_min_frame_time.as<kit::time::nanoseconds, long long>() > 0;
    return capped ? (std::uint32_t)(1.f / m_min_frame_time.as<kit::time::seconds, float>()) : 0;
}

template <typename Dim> void app<Dim>::limit_framerate(const std::uint32_t framerate)
{
    m_min_frame_time = kit::time::from<kit::time::seconds>(framerate > 0 ? (1.f / framerate) : 0.f);
}

#ifdef LYNX_ENABLE_IMGUI
template <typename Dim> void app<Dim>::imgui_init()
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

    KIT_CHECK_RETURN_VALUE(
        vkCreateDescriptorPool(m_window->device()->vulkan_device(), &pool_info, nullptr, &m_imgui_pool), VK_SUCCESS,
        CRITICAL, "Failed to create descriptor pool")

    m_imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imgui_context);
#ifdef LYNX_ENABLE_IMPLOT
    m_implot_context = ImPlot::CreateContext();
    ImPlot::SetCurrentContext(m_implot_context);
#endif

    IMGUI_CHECKVERSION();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForVulkan(m_window->glfw_window(), true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = m_window->device()->vulkan_instance();
    init_info.PhysicalDevice = m_window->device()->vulkan_physical_device();
    init_info.Device = m_window->device()->vulkan_device();
    init_info.Queue = m_window->device()->graphics_queue();
    init_info.DescriptorPool = m_imgui_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, m_window->renderer().swap_chain().render_pass());

    m_window->renderer().immediate_submission(
        [](const VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

template <typename Dim> void app<Dim>::imgui_begin_render()
{
    KIT_PERF_FUNCTION()
    ImGui::SetCurrentContext(m_imgui_context);
#ifdef LYNX_ENABLE_IMPLOT
    ImPlot::SetCurrentContext(m_implot_context);
#endif

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();

    ImGui::NewFrame();
    ImGui::PushID(this);
}

template <typename Dim> void app<Dim>::imgui_end_render()
{
    KIT_PERF_FUNCTION()
    ImGui::PopID();
    ImGui::Render();
}

template <typename Dim> void app<Dim>::imgui_submit_command(const VkCommandBuffer command_buffer)
{
    KIT_PERF_FUNCTION()
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(current_context);
    }
}

template <typename Dim> void app<Dim>::imgui_shutdown()
{
    ImGui::SetCurrentContext(m_imgui_context);
#ifdef LYNX_ENABLE_IMPLOT
    ImPlot::SetCurrentContext(m_implot_context);
#endif

    vkDestroyDescriptorPool(m_window->device()->vulkan_device(), m_imgui_pool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
#ifdef LYNX_ENABLE_IMPLOT
    ImPlot::DestroyContext(m_implot_context);
#endif
    ImGui::DestroyContext(m_imgui_context);
}
#endif

#ifdef KIT_USE_YAML_CPP
template <typename Dim> YAML::Node app<Dim>::encode() const
{
    return kit::yaml::codec<app<Dim>>::encode(*this);
}
template <typename Dim> bool app<Dim>::decode(const YAML::Node &node)
{
    return kit::yaml::codec<app<Dim>>::decode(node, *this);
}
#endif

template class app<dimension::two>;
template class app<dimension::three>;

} // namespace lynx