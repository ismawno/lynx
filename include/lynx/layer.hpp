#ifndef LYNX_LAYER_HPP
#define LYNX_LAYER_HPP

#include <functional>
#include <vulkan/vulkan.hpp>

namespace lynx
{
class app;
class layer
{
  public:
    layer(const char *name);

  private:
    const char *m_name;
    virtual void on_attach()
    {
    }

    virtual void on_detach()
    {
    }

    virtual void on_update(float ts)
    {
    }

    virtual void on_command_submission(VkCommandBuffer command_buffer)
    {
    }

    friend class app;
};

// class imgui_layer : public layer
// {
//   public:
//     using layer::layer;

//   protected:
//     virtual void on_attach() override;
//     virtual void on_detach() override;
//     virtual void on_update(float ts) override;
//     virtual void on_command_submission(VkCommandBuffer command_buffer) override;
// };
} // namespace lynx

#endif