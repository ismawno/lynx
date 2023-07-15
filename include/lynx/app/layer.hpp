#ifndef LYNX_LAYER_HPP
#define LYNX_LAYER_HPP

#include "lynx/app/input.hpp"
#include "kit/interface/non_copyable.hpp"
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
    template <typename T = app> T *parent() const
    {
        if constexpr (std::is_same<T, app>::value)
            return m_parent;
        else
            return dynamic_cast<T *>(m_parent);
    }

  private:
    const char *m_name;
    app *m_parent = nullptr;
    virtual void on_attach()
    {
    }
    virtual void on_detach()
    {
    }
    virtual void on_update(float ts)
    {
    }
    virtual void on_render(float ts)
    {
    }
    virtual bool on_event(event ev)
    {
        return false;
    }
    virtual void on_command_submission(VkCommandBuffer command_buffer)
    {
    }

    friend class app;
};

class imgui_layer : public layer, kit::non_copyable
{
  public:
    imgui_layer(const char *name = "ImGui Layer");
    virtual ~imgui_layer() = default;

  protected:
    virtual void on_attach() override;
    virtual void on_detach() override;
    void on_render(float ts) override;
    virtual void on_command_submission(VkCommandBuffer command_buffer) override;

    virtual void on_imgui_render() = 0;

  private:
    VkDescriptorPool m_imgui_pool;
    ImGuiContext *m_imgui_context;
};
} // namespace lynx

#endif