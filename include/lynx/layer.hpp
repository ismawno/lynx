#ifndef LYNX_LAYER_HPP
#define LYNX_LAYER_HPP

#include <functional>
#include <vulkan/vulkan.hpp>
#include <imgui.h>

namespace lynx
{
class app;
class layer
{
  public:
    layer(const char *name);

    const char *name() const;

  private:
    const char *m_name;
    virtual void on_attach(app *parent)
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

class imgui_layer : public layer
{
  public:
    imgui_layer(const char *name = "ImGui Layer");
    virtual ~imgui_layer() = default;

  protected:
    virtual void on_attach(app *parent) override;
    virtual void on_detach() override;
    void on_update(float ts) override;
    virtual void on_command_submission(VkCommandBuffer command_buffer) override;

    virtual void on_imgui_render() = 0;

  private:
    VkDescriptorPool m_imgui_pool;
    ImGuiContext *m_imgui_context;
    app *m_parent;

    imgui_layer(const imgui_layer &) = delete;
    imgui_layer &operator=(const imgui_layer &) = delete;
};
} // namespace lynx

#endif