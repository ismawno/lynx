#ifndef LYNX_LAYER_HPP
#define LYNX_LAYER_HPP

#include "lynx/app/input.hpp"
#include "kit/interface/nameable.hpp"
#include "kit/interface/toggleable.hpp"
#include "kit/interface/serializable.hpp"
#include <functional>
#include <vulkan/vulkan.hpp>

namespace lynx
{
class app;
class layer : public kit::nameable, public kit::toggleable, public kit::serializable
{
  public:
    using kit::nameable::nameable;

    virtual ~layer() = default;

    template <typename T = app> T *parent() const
    {
        if constexpr (std::is_same<T, app>::value)
            return m_parent;
        else
            return dynamic_cast<T *>(m_parent);
    }

  private:
    app *m_parent = nullptr;

#ifdef KIT_USE_YAML_CPP
    virtual YAML::Node encode() const;
    virtual bool decode(const YAML::Node &node);
#endif

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
    virtual bool on_event(event ev)
    {
        return false;
    }
    virtual void on_command_submission(VkCommandBuffer command_buffer)
    {
    }

    friend class app;
};
} // namespace lynx

#endif