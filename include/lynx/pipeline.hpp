#ifndef LYNX_PIPELINE_HPP
#define LYNX_PIPELINE_HPP

#include "lynx/device.hpp"
#include <vector>

namespace lynx
{
class pipeline
{
  public:
    struct config_info
    {
        config_info() = default;
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
        VkPipelineLayout pipeline_layout = nullptr;
        VkRenderPass render_pass = nullptr;
        std::uint32_t subpass = 0;
        static config_info default_config(std::uint32_t width, std::uint32_t height);

        config_info(const config_info &) = delete;
        config_info &operator=(const config_info &) = delete;

        config_info(config_info &&) = default;
        config_info &operator=(config_info &&) = default;
    };

    pipeline(const device &dev, const char *vert_path, const char *frag_path, const config_info &config);
    ~pipeline();

    void bind(VkCommandBuffer command_buffer) const;

  private:
    const device &m_device;
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;

    void init(const char *vert_path, const char *frag_path, const config_info &config);
    void create_shader_module(const std::vector<char> &code, VkShaderModule *shader_module) const;

    static std::vector<char> read_file(const char *path);

    pipeline(const pipeline &) = delete;
    void operator=(const pipeline &) = delete;
};
} // namespace lynx

#endif