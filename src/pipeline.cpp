#include "lynx/pch.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/exceptions.hpp"
#include "lynx/model.hpp"

#include <fstream>

namespace lynx
{
pipeline::pipeline(const device &dev, const char *vert_path, const char *frag_path, const config_info &config)
    : m_device(dev)
{
    init(vert_path, frag_path, config);
}

pipeline::~pipeline()
{
    vkDestroyShaderModule(m_device.vulkan_device(), m_vert_shader_module, nullptr);
    vkDestroyShaderModule(m_device.vulkan_device(), m_frag_shader_module, nullptr);
    vkDestroyPipeline(m_device.vulkan_device(), m_graphics_pipeline, nullptr);
}

void pipeline::bind(VkCommandBuffer command_buffer) const
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
}

void pipeline::init(const char *vert_path, const char *frag_path, const config_info &config)
{
    DBG_ASSERT_ERROR(config.pipeline_layout, "Pipeline layout must be provided to create graphics pipeline!")
    DBG_ASSERT_ERROR(config.render_pass, "Render pass must be provided to create graphics pipeline!")

    const std::vector<char> vert_code = read_file(vert_path);
    const std::vector<char> frag_code = read_file(frag_path);

    create_shader_module(vert_code, &m_vert_shader_module);
    create_shader_module(frag_code, &m_frag_shader_module);

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages;
    for (auto &shader_stage : shader_stages)
    {
        shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage.pName = "main";
        shader_stage.flags = 0;
        shader_stage.pNext = nullptr;
        shader_stage.pSpecializationInfo = nullptr;
    }
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = m_vert_shader_module;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = m_frag_shader_module;

    const auto binding_description = model::vertex::binding_descriptions();
    const auto attribute_description = model::vertex::attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount = (std::uint32_t)attribute_description.size();
    vertex_input_info.vertexBindingDescriptionCount = (std::uint32_t)binding_description.size();
    vertex_input_info.pVertexAttributeDescriptions = attribute_description.data();
    vertex_input_info.pVertexBindingDescriptions = binding_description.data();

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &config.input_assembly_info;
    pipeline_info.pViewportState = &config.viewport_info;
    pipeline_info.pRasterizationState = &config.rasterization_info;
    pipeline_info.pMultisampleState = &config.multisample_info;
    pipeline_info.pColorBlendState = &config.color_blend_info;
    pipeline_info.pDepthStencilState = &config.depth_stencil_info;
    pipeline_info.pDynamicState = &config.dynamic_state_info;

    pipeline_info.layout = config.pipeline_layout;
    pipeline_info.renderPass = config.render_pass;
    pipeline_info.subpass = config.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    if (vkCreateGraphicsPipelines(m_device.vulkan_device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                  &m_graphics_pipeline) != VK_SUCCESS)
        throw bad_init("Failed to create graphics pipeline");
}

void pipeline::create_shader_module(const std::vector<char> &code, VkShaderModule *shader_module) const
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = (const std::uint32_t *)code.data();
    if (vkCreateShaderModule(m_device.vulkan_device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
        throw bad_init("Failed to create shader module");
}

void pipeline::config_info::default_config(config_info &config)
{
    config.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    config.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.viewport_info.viewportCount = 1;
    config.viewport_info.pViewports = nullptr;
    config.viewport_info.scissorCount = 1;
    config.viewport_info.pScissors = nullptr;

    config.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterization_info.depthClampEnable = VK_FALSE;
    config.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    config.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterization_info.lineWidth = 1.0f;
    config.rasterization_info.cullMode = VK_CULL_MODE_NONE;
    config.rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    config.rasterization_info.depthBiasEnable = VK_FALSE;
    config.rasterization_info.depthBiasConstantFactor = 0.0f; // Optional
    config.rasterization_info.depthBiasClamp = 0.0f;          // Optional
    config.rasterization_info.depthBiasSlopeFactor = 0.0f;    // Optional
    config.rasterization_info.pNext = nullptr;
    config.rasterization_info.flags = 0;

    config.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisample_info.sampleShadingEnable = VK_FALSE;
    config.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisample_info.minSampleShading = 1.0f;          // Optional
    config.multisample_info.pSampleMask = nullptr;            // Optional
    config.multisample_info.alphaToCoverageEnable = VK_FALSE; // Optional
    config.multisample_info.alphaToOneEnable = VK_FALSE;      // Optional
    config.multisample_info.pNext = nullptr;
    config.multisample_info.flags = 0;

    config.color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config.color_blend_attachment.blendEnable = VK_FALSE;
    config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

    config.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config.color_blend_info.logicOpEnable = VK_FALSE;
    config.color_blend_info.logicOp = VK_LOGIC_OP_COPY; // Optional
    config.color_blend_info.attachmentCount = 1;
    config.color_blend_info.pAttachments = &config.color_blend_attachment;
    config.color_blend_info.blendConstants[0] = 0.0f; // Optional
    config.color_blend_info.blendConstants[1] = 0.0f; // Optional
    config.color_blend_info.blendConstants[2] = 0.0f; // Optional
    config.color_blend_info.blendConstants[3] = 0.0f; // Optional
    config.color_blend_info.pNext = nullptr;
    config.color_blend_info.flags = 0;

    config.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.depth_stencil_info.depthTestEnable = VK_TRUE;
    config.depth_stencil_info.depthWriteEnable = VK_TRUE;
    config.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    config.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    config.depth_stencil_info.minDepthBounds = 0.0f; // Optional
    config.depth_stencil_info.maxDepthBounds = 1.0f; // Optional
    config.depth_stencil_info.stencilTestEnable = VK_FALSE;
    config.depth_stencil_info.front = {}; // Optional
    config.depth_stencil_info.back = {};  // Optional
    config.depth_stencil_info.pNext = nullptr;
    config.depth_stencil_info.flags = 0;

    config.dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    config.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config.dynamic_state_info.pDynamicStates = config.dynamic_state_enables.data();
    config.dynamic_state_info.dynamicStateCount = (std::uint32_t)config.dynamic_state_enables.size();
}

std::vector<char> pipeline::read_file(const char *path)
{
    std::ifstream file{path, std::ios::ate | std::ios::binary};
    if (!file.is_open())
        throw file_not_found_error(path);

    const long file_size = file.tellg();
    std::vector<char> buffer((std::size_t)file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);
    return buffer;
}
} // namespace lynx