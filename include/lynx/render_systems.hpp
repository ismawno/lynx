#ifndef LYNX_RENDER_SYSTEM_HPP
#define LYNX_RENDER_SYSTEM_HPP

#include "lynx/core.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/model.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;

class render_system
{
  public:
    virtual ~render_system();

    void init(const device *dev, VkRenderPass render_pass);
    virtual void render(VkCommandBuffer command_buffer) const = 0;

  protected:
    const device *m_device = nullptr;

    void create_pipeline_layout(const pipeline::config_info &config);
    void create_pipeline(VkRenderPass render_pass, pipeline::config_info &config);

    template <typename T> void render(VkCommandBuffer command_buffer, const model &mdl, const T &push_data) const
    {
        DBG_ASSERT_CRITICAL(m_device, "Render system must be properly initialized before rendering!")
        m_pipeline->bind(command_buffer);
        vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(T), &push_data);

        mdl.bind(command_buffer);
        mdl.draw(command_buffer);
    }

    virtual void pipeline_config(pipeline::config_info &config) const;

  private:
    scope<pipeline> m_pipeline;
    VkPipelineLayout m_pipeline_layout;
};

class render_system2D : public render_system
{
  public:
    model2D &push_model(const std::vector<vertex2D> &vertices);

    void render(VkCommandBuffer command_buffer) const override;
    void clear_models();

  protected:
    virtual void pipeline_config(pipeline::config_info &config) const override;

  private:
    std::vector<scope<model2D>> m_models;
};

class render_system3D : public render_system
{
  public:
    model3D &push_model(const std::vector<vertex3D> &vertices);

    void render(VkCommandBuffer command_buffer) const override;
    void clear_models();

  protected:
    virtual void pipeline_config(pipeline::config_info &config) const override;

  private:
    std::vector<scope<model3D>> m_models;
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

class line_render_system3D : public render_system3D
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class line_strip_render_system3D : public render_system3D
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class triangle_render_system3D : public render_system3D
{
    void pipeline_config(pipeline::config_info &config) const override;
};

class triangle_strip_render_system3D : public render_system3D
{
    void pipeline_config(pipeline::config_info &config) const override;
};
} // namespace lynx

#endif