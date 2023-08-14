#include "lynx/internal/pch.hpp"
#include "lynx/rendering/render_system.hpp"
#include "lynx/rendering/device.hpp"
#include "lynx/geometry/vertex.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/rendering/buffer.hpp"
#include "lynx/drawing/drawable.hpp"
#include "kit/profile/perf.hpp"

#define VERTEX_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.vert.spv"
#define FRAGMENT_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.frag.spv"

#define VERTEX_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.vert.spv"
#define FRAGMENT_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.frag.spv"

namespace lynx
{
render_system::~render_system()
{
    if (m_device)
        vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
}

void render_system::init(const kit::ref<const device> &dev, VkRenderPass render_pass)
{
    m_device = dev;

    pipeline::config_info config{};
    pipeline_config(config);

    create_pipeline_layout(config);
    create_pipeline(render_pass, config);
}

void render_system::create_pipeline_layout(const pipeline::config_info &config)
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

void render_system::create_pipeline(const VkRenderPass render_pass, pipeline::config_info &config)
{
    KIT_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!");

    config.render_pass = render_pass;
    config.pipeline_layout = m_pipeline_layout;
    m_pipeline = kit::make_scope<pipeline>(m_device, config);
}

void render_system::render(VkCommandBuffer command_buffer, const camera &cam) const
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

render_data render_system::create_render_data(const model *mdl, glm::mat4 &mdl_transform, const bool unowned) const
{
    KIT_ASSERT_CRITICAL(mdl, "Model cannot be a null pointer")
#ifdef DEBUG
    mdl->to_be_rendered = true;
#endif
    return {mdl, mdl_transform, unowned};
}

void render_system::push_render_data(const render_data &rdata)
{
    m_render_data.push_back(rdata);
}

void render_system::clear_render_data()
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

void render_system::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.constant_range_size = sizeof(push_constant_data);
}

void render_system2D::draw(const std::vector<vertex2D> &vertices, const kit::transform2D &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const render_data rdata = create_render_data(new model2D(m_device, vertices), mdl_mat, true);
    push_render_data(rdata);
}

void render_system2D::draw(const std::vector<vertex2D> &vertices, const std::vector<std::uint32_t> &indices,
                           const kit::transform2D &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const render_data rdata = create_render_data(new model2D(m_device, vertices, indices), mdl_mat, true);
    push_render_data(rdata);
}

void render_system2D::draw(const drawable2D &drawable)
{
    drawable.draw(*this);
}

render_data render_system2D::create_render_data(const model *mdl, glm::mat4 &mdl_transform, const bool unowned) const
{
    const float z_offset = 1.f - ++s_z_offset_counter * std::numeric_limits<float>::epsilon();
    mdl_transform[3][2] = z_offset;
    return render_system::create_render_data(mdl, mdl_transform, unowned);
}

void render_system2D::reset_z_offset_counter()
{
    s_z_offset_counter = 0;
}

void render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system::pipeline_config(config);
    config.vertex_shader_path = VERTEX_SHADER_2D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_2D_PATH;
    config.binding_descriptions = vertex2D::binding_descriptions();
    config.attribute_descriptions = vertex2D::attribute_descriptions();
}

void render_system3D::draw(const std::vector<vertex3D> &vertices, const kit::transform3D &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const render_data rdata = create_render_data(new model3D(m_device, vertices), mdl_mat, true);
    push_render_data(rdata);
}

void render_system3D::draw(const std::vector<vertex3D> &vertices, const std::vector<std::uint32_t> &indices,
                           const kit::transform3D &transform)
{
    glm::mat4 mdl_mat = transform.center_scale_rotate_translate4();
    const render_data rdata = create_render_data(new model3D(m_device, vertices, indices), mdl_mat, true);
    push_render_data(rdata);
}

void render_system3D::draw(const drawable3D &drawable)
{
    drawable.draw(*this);
}

void render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system::pipeline_config(config);
    config.vertex_shader_path = VERTEX_SHADER_3D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_3D_PATH;
    config.binding_descriptions = vertex3D::binding_descriptions();
    config.attribute_descriptions = vertex3D::attribute_descriptions();
}

void point_render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system2D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

void line_render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system2D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
}

void line_strip_render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system2D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
}

void triangle_render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system2D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void triangle_strip_render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system2D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
}

void point_render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system3D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

void line_render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system3D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
}

void line_strip_render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system3D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
}

void triangle_render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system3D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void triangle_strip_render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system3D::pipeline_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
}
} // namespace lynx