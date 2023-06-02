#ifndef LYNX_RENDER_SYSTEM_HPP
#define LYNX_RENDER_SYSTEM_HPP

#include "lynx/core.hpp"
#include "lynx/pipeline.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
class model;
class render_system
{
  public:
    virtual ~render_system();

    void init(const ref<const device> &dev, VkRenderPass render_pass);
    void render(VkCommandBuffer command_buffer, const model &mdl) const;

  protected:
    void create_pipeline_layout();
    void create_pipeline(VkRenderPass render_pass);

  private:
    ref<const device> m_device;
    scope<pipeline> m_pipeline;
    VkPipelineLayout m_pipeline_layout;

    virtual void pipeline_config(pipeline::config_info &config) const = 0;
};

class line_render_system : public render_system
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class line_strip_render_system : public render_system
{
    void pipeline_config(pipeline::config_info &config) const override;
};
} // namespace lynx

#endif