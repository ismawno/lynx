#ifndef LYNX_LAYER_HPP
#define LYNX_LAYER_HPP

#include "lynx/app/input.hpp"
#include "kit/interface/nameable.hpp"
#include "kit/interface/toggleable.hpp"
#include <functional>
#include <vulkan/vulkan.hpp>

namespace lynx
{
class app;
class layer : public kit::nameable, public kit::toggleable
{
  public:
    template <typename T = app> T *parent() const
    {
        if constexpr (std::is_same<T, app>::value)
            return m_parent;
        else
            return dynamic_cast<T *>(m_parent);
    }

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