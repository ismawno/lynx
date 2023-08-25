#ifndef LYNX_LAYER_HPP
#define LYNX_LAYER_HPP

#include "lynx/app/input.hpp"
#include "kit/interface/identifiable.hpp"
#include "kit/interface/toggleable.hpp"
#include "kit/interface/serialization.hpp"
#include <functional>
#include <vulkan/vulkan.hpp>

namespace lynx
{
class app;
class layer : public kit::identifiable<std::string>, public kit::toggleable, public kit::serializable
{
  public:
    layer(const std::string &name);
    virtual ~layer() = default;

    template <typename T = app> T *parent() const
    {
        if constexpr (std::is_same_v<T, app>)
            return m_parent;
        else
            return dynamic_cast<T *>(m_parent);
    }

#ifdef KIT_USE_YAML_CPP
    virtual YAML::Node encode() const;
    virtual bool decode(const YAML::Node &node);
#endif

  private:
    app *m_parent = nullptr;

    virtual void on_attach()
    {
    }
    virtual void on_detach()
    {
    }
    virtual void on_start()
    {
    }
    virtual void on_update(float ts)
    {
    }
    virtual void on_render(float ts)
    {
    }
    virtual void on_frame_end(float ts)
    {
    }
    virtual bool on_event(const event &ev)
    {
        return false;
    }
    virtual void on_shutdown()
    {
    }
    virtual void on_command_submission(VkCommandBuffer command_buffer)
    {
    }

    friend class app;
};
} // namespace lynx

#endif