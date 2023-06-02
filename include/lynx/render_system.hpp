#ifndef LYNX_RENDER_SYSTEM_HPP
#define LYNX_RENDER_SYSTEM_HPP

#include "lynx/core.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class device;
class pipeline;
class model;
class render_system
{
  public:
    render_system(const ref<const device> &dev, VkRenderPass render_pass);
    ~render_system();

    void render(VkCommandBuffer command_buffer, const model &mdl) const;

  private:
    ref<const device> m_device;
    scope<pipeline> m_pipeline;
    VkPipelineLayout m_pipeline_layout;

    void create_pipeline_layout();
    void create_pipeline(VkRenderPass render_pass);
};
} // namespace lynx

#endif