#include "lynx/pch.hpp"
#include "lynx/render_systems.hpp"
#include "lynx/device.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/model.hpp"

#define VERTEX_SHADER_2D_PATH LYNX_SHADER_PATH "shader.vert.spv"
#define FRAGMENT_SHADER_2D_PATH LYNX_SHADER_PATH "shader.frag.spv"

#define VERTEX_SHADER_3D_PATH LYNX_SHADER_PATH "shader.vert.spv"
#define FRAGMENT_SHADER_3D_PATH LYNX_SHADER_PATH "shader.frag.spv"

namespace lynx
{
struct push_constant_data2D
{
    glm::mat2 transform{1.f};
    glm::vec2 offset{0.f};
    alignas(16) glm::vec3 color{.2f};
};

struct push_constant_data3D
{
    glm::mat3 transform{1.f};
    glm::vec3 offset{0.f};
    alignas(16) glm::vec3 color{.2f};
};

render_system::~render_system()
{
    if (m_device)
        vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
}

void render_system::init(const device *dev, VkRenderPass render_pass)
{
    m_device = dev;
    pipeline::config_info pip_config{};
    pipeline_config(pip_config);

    create_pipeline_layout(pip_config);
    create_pipeline(render_pass, pip_config);
}

template <typename T>
void render_system::render(const VkCommandBuffer command_buffer, const model &mdl, const T &push_data) const
{
    DBG_ASSERT_CRITICAL(m_device, "Render system must be properly initialized before rendering!")
    m_pipeline->bind(command_buffer);
    vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(T), &push_data);

    mdl.bind(command_buffer);
    mdl.draw(command_buffer);
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
    DBG_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!")
    config.render_pass = render_pass;
    config.pipeline_layout = m_pipeline_layout;
    m_pipeline = make_scope<pipeline>(*m_device, config);
}

void render_system2D::render(VkCommandBuffer command_buffer, const model2D &mdl) const
{
    static int frame = 0;
    const float angle = frame++ / (20.f * (float)M_PI);
    const float c = cosf(angle), s = sinf(angle);
    push_constant_data2D push_data{};
    push_data.transform = glm::mat2{{c, s}, {-s, c}};
    push_data.offset = {0.4f, 0.f};
    render_system::render(command_buffer, mdl, push_data);
}

void render_system2D::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.vertex_shader_path = VERTEX_SHADER_2D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_2D_PATH;
    config.is_2D = true;
    config.constant_range_size = sizeof(push_constant_data2D);
}

void render_system3D::render(VkCommandBuffer command_buffer, const model3D &mdl) const
{
    push_constant_data3D push_data{};
    render_system::render(command_buffer, mdl, push_data);
}

void render_system3D::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.vertex_shader_path = VERTEX_SHADER_3D_PATH;
    config.fragment_shader_path = FRAGMENT_SHADER_3D_PATH;
    config.is_2D = false;
    config.constant_range_size = sizeof(push_constant_data3D);
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
} // namespace lynx