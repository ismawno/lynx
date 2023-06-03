#ifndef LYNX_RENDER_SYSTEM_HPP
#define LYNX_RENDER_SYSTEM_HPP

#include "lynx/core.hpp"
#include "lynx/pipeline.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
class model;
class model2D;
class model3D;
class render_system
{
  public:
    virtual ~render_system();

    void init(const device *dev, VkRenderPass render_pass);

  protected:
    void create_pipeline_layout();
    void create_pipeline(VkRenderPass render_pass);
    void render(VkCommandBuffer command_buffer, const model &mdl) const;

  private:
    const device *m_device = nullptr;
    scope<pipeline> m_pipeline;
    VkPipelineLayout m_pipeline_layout;

    virtual void pipeline_config(pipeline::config_info &config) const = 0;
};

class render_system2D : public render_system
{
  public:
    void render(VkCommandBuffer command_buffer, const model2D &mdl) const;

  protected:
    virtual void pipeline_config(pipeline::config_info &config) const override;
};

class render_system3D : public render_system
{
  public:
    void render(VkCommandBuffer command_buffer, const model3D &mdl) const;

  protected:
    virtual void pipeline_config(pipeline::config_info &config) const override;
};

class line_render_system2D : public render_system2D
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class line_strip_render_system2D : public render_system2D
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class triangle_render_system2D : public render_system2D
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class triangle_strip_render_system2D : public render_system2D
{
    void pipeline_config(pipeline::config_info &config) const override;
};
} // namespace lynx

#endif