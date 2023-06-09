#include "lynx/pch.hpp"
#include "lynx/render_systems.hpp"
#include "lynx/device.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/vertex.hpp"
#include "lynx/primitives.hpp"
#include "lynx/camera.hpp"

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

void render_system::init(const ref<const device> &dev, VkRenderPass render_pass)
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
    if (vkCreatePipelineLayout(m_device->vulkan_device(), &layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        throw bad_init("Failed to create pipeline layout");
}

void render_system::create_pipeline(const VkRenderPass render_pass, pipeline::config_info &config)
{
    DBG_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!");

    config.render_pass = render_pass;
    config.pipeline_layout = m_pipeline_layout;
    m_pipeline = make_scope<pipeline>(m_device, config);
}

void render_system::render(VkCommandBuffer command_buffer, const camera3D &cam) const
{
    const glm::mat4 proj = cam.projection_matrix() * cam.view_matrix();
    for (const auto &[mdl, push_data] : m_render_data)
    {
        DBG_ASSERT_CRITICAL(m_device, "Render system must be properly initialized before rendering!")
        m_pipeline->bind(command_buffer);

        const push_constant_data push_with_camera = {proj * push_data.transform};
        vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(push_constant_data), &push_with_camera);

        mdl->bind(command_buffer);
        mdl->draw(command_buffer);
    }
}

void render_system::push_render_data(const render_data &rdata)
{
    m_render_data.push_back(rdata);
}

void render_system::clear_render_data()
{
    m_render_data.clear();
}

void render_system::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.constant_range_size = sizeof(push_constant_data);
}

void render_system2D::draw(const std::vector<vertex2D> &vertices, const transform2D &transform)
{
    push_render_data({make_ref<model2D>(m_device, vertices), {transform}});
}

void render_system2D::draw(const drawable2D &drawable)
{
    drawable.draw(*this);
}

ref<model2D> render_system2D::model_from_vertices(const std::vector<vertex2D> &vertices) const
{
    return make_ref<model2D>(m_device, vertices);
}

void render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system::pipeline_config(config);
    config.vertex_shader_path = VERTEX_SHADER_2D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_2D_PATH;
    config.binding_descriptions = vertex2D::binding_descriptions();
    config.attribute_descriptions = vertex2D::attribute_descriptions();
}

void render_system3D::draw(const std::vector<vertex3D> &vertices, const transform3D &transform)
{
    push_render_data({make_ref<model3D>(m_device, vertices), {transform}});
}

void render_system3D::draw(const drawable3D &drawable)
{
    drawable.draw(*this);
}

ref<model3D> render_system3D::model_from_vertices(const std::vector<vertex3D> &vertices) const
{
    return make_ref<model3D>(m_device, vertices);
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