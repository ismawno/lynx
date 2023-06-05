#include "lynx/pch.hpp"
#include "lynx/render_systems.hpp"
#include "lynx/device.hpp"
#include "lynx/exceptions.hpp"

#define VERTEX_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.vert.spv"
#define FRAGMENT_SHADER_2D_PATH LYNX_SHADER_PATH "bin/shader2D.frag.spv"

#define VERTEX_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.vert.spv"
#define FRAGMENT_SHADER_3D_PATH LYNX_SHADER_PATH "bin/shader3D.frag.spv"

namespace lynx
{
struct push_constant_data
{
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color{.2f};
};

struct transform2D
{
    glm::vec2 translation{0.f};
    glm::vec2 scale{1.f};
    float rotation = 0.f;
    operator glm::mat4()
    {
        const float c = cosf(rotation);
        const float s = sinf(rotation);
        return glm::mat4{{
                             scale.x * c,
                             scale.x * s,
                             0.f,
                             0.f,
                         },
                         {
                             -scale.y * s,
                             scale.y * c,
                             0.f,
                             0.f,
                         },
                         {
                             0.f,
                             0.f,
                             0.f,
                             0.f,
                         },
                         {translation.x, translation.y, 0.f, 1.0f}};
    }
};

struct transform3D
{
    glm::vec3 translation{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 rotation{0.f};
    operator glm::mat4()
    {
        const float c3 = cosf(rotation.z);
        const float s3 = sinf(rotation.z);
        const float c2 = cosf(rotation.x);
        const float s2 = sinf(rotation.x);
        const float c1 = cosf(rotation.y);
        const float s1 = sinf(rotation.y);
        return glm::mat4{{
                             scale.x * (c1 * c3 + s1 * s2 * s3),
                             scale.x * (c2 * s3),
                             scale.x * (c1 * s2 * s3 - c3 * s1),
                             0.f,
                         },
                         {
                             scale.y * (c3 * s1 * s2 - c1 * s3),
                             scale.y * (c2 * c3),
                             scale.y * (c1 * c3 * s2 + s1 * s3),
                             0.f,
                         },
                         {
                             scale.z * (c2 * s1),
                             scale.z * (-s2),
                             scale.z * (c1 * c2),
                             0.f,
                         },
                         {translation.x, translation.y, translation.z, 1.0f}};
    }
};

render_system::~render_system()
{
    if (m_device)
        vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
}

void render_system::init(const device *dev, VkRenderPass render_pass)
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
    m_pipeline = make_scope<pipeline>(*m_device, config);
}

void render_system::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.constant_range_size = sizeof(push_constant_data);
}

void render_system2D::render(VkCommandBuffer command_buffer, const model2D &mdl) const
{
    static int frame = 0;
    transform2D trans{};
    trans.rotation = frame++ / (20.f * (float)M_PI);

    push_constant_data push_data{};
    push_data.transform = trans;
    render_system::render(command_buffer, mdl, push_data);
}

void render_system2D::pipeline_config(pipeline::config_info &config) const
{
    render_system::pipeline_config(config);
    config.vertex_shader_path = VERTEX_SHADER_2D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_2D_PATH;
    config.binding_descriptions = model2D::vertex::binding_descriptions();
    config.attribute_descriptions = model2D::vertex::attribute_descriptions();
}

void render_system3D::render(VkCommandBuffer command_buffer, const model3D &mdl) const
{
    static int frame = 0;
    transform3D trans{};
    trans.rotation.x = frame++ / (20.f * (float)M_PI);
    trans.rotation.y = frame / (20.f * (float)M_PI);
    trans.translation.z = 0.5f;
    trans.scale = glm::vec3{0.5f};

    push_constant_data push_data{};
    push_data.transform = trans;
    render_system::render(command_buffer, mdl, push_data);
}

void render_system3D::pipeline_config(pipeline::config_info &config) const
{
    render_system::pipeline_config(config);
    config.vertex_shader_path = VERTEX_SHADER_3D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_3D_PATH;
    config.binding_descriptions = model3D::vertex::binding_descriptions();
    config.attribute_descriptions = model3D::vertex::attribute_descriptions();
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