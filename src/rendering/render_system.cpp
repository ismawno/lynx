#include "lynx/internal/pch.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/device.hpp"
#include "lynx/geometry/vertex.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/rendering/buffer.hpp"

#define VERTEX_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.vert.spv"
#define FRAGMENT_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.frag.spv"

#define VERTEX_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.vert.spv"
#define FRAGMENT_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.frag.spv"

namespace lynx
{
template <Dimension Dim> render_system<Dim>::~render_system()
{
    if (m_device)
        vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
}

template <Dimension Dim> void render_system<Dim>::init(const kit::ref<const device> &dev, VkRenderPass render_pass)
{
    m_device = dev;

    pipeline::config_info config{};
    pipeline_config(config);

    create_pipeline_layout(config);
    create_pipeline(render_pass, config);
}

template <Dimension Dim> void render_system<Dim>::create_pipeline_layout(const pipeline::config_info &config)
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

template <Dimension Dim>
void render_system<Dim>::create_pipeline(const VkRenderPass render_pass, pipeline::config_info &config)
{
    KIT_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!");

    config.render_pass = render_pass;
    config.pipeline_layout = m_pipeline_layout;
    m_pipeline = kit::make_scope<pipeline>(m_device, config);
}

template <Dimension Dim> void render_system<Dim>::render(VkCommandBuffer command_buffer, const camera_t &cam) const
{
    if (m_render_data.empty())
        return;

    KIT_PERF_SCOPE("lynx::render_system::render")
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

template <Dimension Dim>
typename render_system<Dim>::render_data render_system<Dim>::create_render_data(const kit::ref<const model_t> &mdl,
                                                                                glm::mat4 &mdl_transform) const
{
    KIT_ASSERT_CRITICAL(mdl, "Model cannot be a null pointer")
    if constexpr (std::is_same_v<Dim, dimension::two>)
    {
        const float z_offset = 1.f - ++s_z_offset_counter2D * std::numeric_limits<float>::epsilon();
        mdl_transform[3][2] = z_offset;
    }
    return {mdl, mdl_transform};
}

template <Dimension Dim> void render_system<Dim>::push_render_data(const render_data &rdata)
{
    m_render_data.push_back(rdata);
}

template <Dimension Dim> void render_system<Dim>::clear_render_data()
{
    m_render_data.clear();
}

template <Dimension Dim> void render_system<Dim>::pipeline_config(pipeline::config_info &config) const
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

template <Dimension Dim>
void render_system<Dim>::draw(const std::vector<vertex_t> &vertices, const transform_t &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const auto model = kit::make_ref<model_t>(m_device, vertices);
    const render_data rdata = create_render_data(model, mdl_mat);
    push_render_data(rdata);
}

template <Dimension Dim>
void render_system<Dim>::draw(const std::vector<vertex_t> &vertices, const std::vector<std::uint32_t> &indices,
                              const transform_t &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const auto model = kit::make_ref<model_t>(m_device, vertices, indices);
    const render_data rdata = create_render_data(model, mdl_mat);
    push_render_data(rdata);
}

template <Dimension Dim> void render_system<Dim>::draw(const drawable_t &drawable)
{
    drawable.draw(*this);
}

template <Dimension Dim> void point_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    render_system<Dim>::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

template <Dimension Dim> void line_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    render_system<Dim>::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
}

template <Dimension Dim> void line_strip_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    render_system<Dim>::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
}

template <Dimension Dim> void triangle_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    render_system<Dim>::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

template <Dimension Dim> void triangle_strip_render_system<Dim>::pipeline_config(pipeline::config_info &config) const
{
    render_system<Dim>::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
}

template class render_system<dimension::two>;
template class render_system<dimension::three>;

template class point_render_system<dimension::two>;
template class point_render_system<dimension::three>;

template class line_render_system<dimension::two>;
template class line_render_system<dimension::three>;

template class line_strip_render_system<dimension::two>;
template class line_strip_render_system<dimension::three>;

template class triangle_render_system<dimension::two>;
template class triangle_render_system<dimension::three>;

template class triangle_strip_render_system<dimension::two>;
template class triangle_strip_render_system<dimension::three>;
} // namespace lynx