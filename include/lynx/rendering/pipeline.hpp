#ifndef LYNX_PIPELINE_HPP
#define LYNX_PIPELINE_HPP

#include "lynx/internal/core.hpp"
#include "lynx/rendering/device.hpp"
#include <vector>

namespace lynx
{
class pipeline
{
  public:
    struct config_info
    {
        config_info() = default;

        VkPipelineViewportStateCreateInfo viewport_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo color_blend_info;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;

        std::vector<VkDynamicState> dynamic_state_enables;
        VkPipelineDynamicStateCreateInfo dynamic_state_info;

        VkPipelineLayout pipeline_layout = nullptr;
        VkRenderPass render_pass = nullptr;
        std::uint32_t subpass = 0;

        const char *vertex_shader_path = nullptr;
        const char *fragment_shader_path = nullptr;

        std::vector<VkVertexInputBindingDescription> binding_descriptions;
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
        std::uint32_t constant_range_size = 0;

        static void default_config(config_info &config);

        config_info(const config_info &) = delete;
        config_info &operator=(const config_info &) = delete;
    };

    pipeline(const ref<const device> &dev, const config_info &config);
    ~pipeline();

    void bind(VkCommandBuffer command_buffer) const;

  private:
    ref<const device> m_device;
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;

    void init(const config_info &config);
    void create_shader_module(const std::vector<char> &code, VkShaderModule *shader_module) const;

    pipeline(const pipeline &) = delete;
    pipeline &operator=(const pipeline &) = delete;
};
} // namespace lynx

#endif