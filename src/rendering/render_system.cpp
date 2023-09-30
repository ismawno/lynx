#include "lynx/internal/pch.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/device.hpp"
#include "lynx/geometry/vertex.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/rendering/buffer.hpp"
#include "kit/profile/perf.hpp"

#define VERTEX_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.vert.spv"
#define FRAGMENT_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.frag.spv"

#define VERTEX_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.vert.spv"
#define FRAGMENT_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.frag.spv"

namespace lynx
{
template <typename Dim> render_system<Dim>::~render_system()
{
    if (m_device)
        vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
}

template <typename Dim> void render_system<Dim>::init(const kit::ref<const device> &dev, VkRenderPass render_pass)
{
    m_device = dev;

    pipeline::config_info config{};
    pipeline_config(config);

    create_pipeline_layout(config);
    create_pipeline(render_pass, config);
}

template <typename Dim> void render_system<Dim>::create_pipeline_layout(const pipeline::config_info &config)
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = config.constant_range_size;

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pSetLayouts = nullptr;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
    KIT_CHECK_RETURN_VALUE(vkCreatePipelineLayout(m_device->vulkan_device(), &layout_info, nullptr, &m_pipeline_layout),
                           VK_SUCCESS, CRITICAL, "Failed to create pipeline layout")
}

template <typename Dim>
void render_system<Dim>::create_pipeline(const VkRenderPass render_pass, pipeline::config_info &config)
{
    KIT_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!");

    config.render_pass = render_pass;
    config.pipeline_layout = m_pipeline_layout;
    m_pipeline = kit::make_scope<pipeline>(m_device, config);
}

template <typename Dim> void render_system<Dim>::render(VkCommandBuffer command_buffer, const camera_t &cam) const
{
    KIT_PERF_PRETTY_FUNCTION()
    if (m_render_data.empty())
        return;

    m_pipeline->bind(command_buffer);
    const glm::mat4 &proj = cam.projection();
    for (const render_data &rdata : m_render_data)
    {
        KIT_ASSERT_CRITICAL(m_device, "Render system must be properly initialized before rendering!")

        const push_constant_data push_with_camera = {rdata.mdl_transform, proj};
        vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(push_constant_data), &push_with_camera);

        rdata.mdl->bind(command_buffer);
        rdata.mdl->draw(command_buffer);
    }
}

template <typename Dim>
typename render_system<Dim>::render_data render_system<Dim>::create_render_data(const model_t *mdl,
                                                                                glm::mat4 &mdl_transform,
                                                                                const bool unowned) const
{
    KIT_ASSERT_CRITICAL(mdl, "Model cannot be a null pointer")
#ifdef DEBUG
    mdl->to_be_rendered = true;
#endif
    return {mdl, mdl_transform, unowned};
}

template <typename Dim> void render_system<Dim>::push_render_data(const render_data &rdata)
{
    m_render_data.push_back(rdata);
}

template <typename Dim> void render_system<Dim>::clear_render_data()
{
    for (const render_data &rdata : m_render_data)
    {
#ifdef DEBUG
        rdata.mdl->to_be_rendered = false;
#endif
        if (rdata.unowned)
            delete rdata.mdl;
    }
    m_render_data.clear();
}

template <typename Dim> void render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.constant_range_size = sizeof(push_constant_data);
    config.binding_descriptions = vertex_t::binding_descriptions();
    config.attribute_descriptions = vertex_t::attribute_descriptions();

    if constexpr (std::is_same_v<Dim, dimension::two>)
    {
        config.vertex_shader_path = VERTEX_SHADER_2D_PATH;
        config.fragment_shader_path = FRAGMENT_SHADER_2D_PATH;
    }
    else
    {
        config.vertex_shader_path = VERTEX_SHADER_3D_PATH;
        config.fragment_shader_path = FRAGMENT_SHADER_3D_PATH;
    }
}

template <typename Dim>
void render_system<Dim>::draw(const std::vector<vertex_t> &vertices, const transform_t &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const render_data rdata = create_render_data(new model_t(m_device, vertices), mdl_mat, true);
    push_render_data(rdata);
}

template <typename Dim>
void render_system<Dim>::draw(const std::vector<vertex_t> &vertices, const std::vector<std::uint32_t> &indices,
                              const transform_t &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const render_data rdata = create_render_data(new model_t(m_device, vertices, indices), mdl_mat, true);
    push_render_data(rdata);
}

template <typename Dim> void render_system<Dim>::draw(const drawable_t &drawable)
{
    drawable.draw(*this);
}

render_system2D::render_data render_system2D::create_render_data(const model_t *mdl, glm::mat4 &mdl_transform,
                                                                 const bool unowned) const
{
    const float z_offset = 1.f - ++s_z_offset_counter * std::numeric_limits<float>::epsilon();
    mdl_transform[3][2] = z_offset;
    return render_system::create_render_data(mdl, mdl_transform, unowned);
}

void render_system2D::reset_z_offset_counter()
{
    s_z_offset_counter = 0;
}

template <typename Dim> void point_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    Dim::render_system_t::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

template <typename Dim> void line_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    Dim::render_system_t::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
}

template <typename Dim> void line_strip_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    Dim::render_system_t::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
}

template <typename Dim> void triangle_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    Dim::render_system_t::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

template <typename Dim> void triangle_strip_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    Dim::render_system_t::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
}
} // namespace lynx