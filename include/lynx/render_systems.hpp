#ifndef LYNX_RENDER_SYSTEM_HPP
#define LYNX_RENDER_SYSTEM_HPP

#include "lynx/core.hpp"
#include "lynx/pipeline.hpp"
#include <vulkan/vulkan.hpp>
#include <utility>

namespace lynx
{
class device;
class model;
struct push_constant_data
{
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color{.2f};
};

struct render_data
{
    ref<const model> mdl;
    push_constant_data push_data;
};
class render_system
{
  public:
    virtual ~render_system();

    void init(const ref<const device> &dev, VkRenderPass render_pass);
    virtual void render(VkCommandBuffer command_buffer) const = 0;

  protected:
    ref<const device> m_device;

    void create_pipeline_layout(const pipeline::config_info &config);
    void create_pipeline(VkRenderPass render_pass, pipeline::config_info &config);

    void render(VkCommandBuffer command_buffer, const render_data &rdata) const;

    void push_render_data(const render_data &rdata);
    void clear_render_data();

    virtual void pipeline_config(pipeline::config_info &config) const;

  private:
    scope<pipeline> m_pipeline;
    VkPipelineLayout m_pipeline_layout;
    std::vector<render_data> m_render_data;
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