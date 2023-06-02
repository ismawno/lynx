#include "lynx/pch.hpp"
#include "lynx/render_systems.hpp"
#include "lynx/device.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/model.hpp"

#define VERTEX_SHADER_PATH LYNX_SHADER_PATH "shader.vert.spv"
#define FRAGMENT_SHADER_PATH LYNX_SHADER_PATH "shader.frag.spv"

namespace lynx
{
struct push_constant_data2D
{
    glm::mat2 transform{1.f};
    glm::vec2 offset{0.f};
    alignas(16) glm::vec3 color{1.f};
};

render_system::render_system(const ref<const device> &dev) : m_device(dev)
{
}

render_system::~render_system()
{
    vkDestroyPipelineLayout(m_device->vulkan_device(), m_pipeline_layout, nullptr);
}

// Hacer render2D aqui con model2D y render3D con model3D
void render_system::render(const VkCommandBuffer command_buffer, const model &mdl) const
{
    m_pipeline->bind(command_buffer);
    push_constant_data2D push_data{};
    vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(push_constant_data2D), &push_data);

    mdl.bind(command_buffer);
    mdl.draw(command_buffer);
}

void render_system::create_pipeline_layout()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(push_constant_data2D);

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pSetLayouts = nullptr;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
    if (vkCreatePipelineLayout(m_device->vulkan_device(), &layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        throw bad_init("Failed to create pipeline layout");
}

void render_system::create_pipeline(const VkRenderPass render_pass)
{
    DBG_ASSERT_ERROR(m_pipeline_layout, "Cannot create pipeline before pipeline layout!")
    pipeline::config_info pip_config{};
    pipeline_config(pip_config);
    pip_config.render_pass = render_pass;
    pip_config.pipeline_layout = m_pipeline_layout;
    m_pipeline = make_scope<pipeline>(m_device, VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH, pip_config);
}

line_render_system::line_render_system(const ref<const device> &dev, VkRenderPass render_pass) : render_system(dev)
{
    create_pipeline_layout();
    create_pipeline(render_pass);
}

void line_render_system::pipeline_config(pipeline::config_info &config) const
{
    pipeline::config_info::default_config(config);
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
}
} // namespace lynx