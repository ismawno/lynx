#pragma once

#include "lynx/app/input.hpp"
#include "lynx/internal/dimension.hpp"
#include "kit/interface/identifiable.hpp"
#include "kit/interface/toggleable.hpp"
#include "kit/serialization/yaml/serializer.hpp"
#include "kit/utility/type_constraints.hpp"

#include <functional>
#include <vulkan/vulkan.hpp>

namespace lynx
{
template <Dimension Dim> class app;

template <Dimension Dim>
class layer : public kit::identifiable<std::string>,
              public kit::toggleable,
              public kit::yaml::serializable,
              public kit::yaml::deserializable
{
  public:
    using app_t = app<Dim>;
    using event_t = event<Dim>;

    layer(const std::string &name);
    template <kit::DerivedFrom<app_t> T = app_t> T *parent() const
    {
        if constexpr (std::is_same_v<app_t, T>)
            return m_parent;
        else
            return dynamic_cast<T *>(m_parent);
    }

    KIT_TOGGLEABLE_FINAL_DEFAULT_SETTER()

#ifdef KIT_USE_YAML_CPP
    virtual YAML::Node encode() const override;
    virtual bool decode(const YAML::Node &node) override;
#endif

  private:
    app_t *m_parent = nullptr;

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
    virtual bool on_event(const event_t &ev)
    {
        return false;
    }
    virtual void on_shutdown()
    {
    }
    virtual void on_command_submission(VkCommandBuffer command_buffer)
    {
    }

    template <Dimension T> friend class app;
};

using layer2D = layer<dimension::two>;
using layer3D = layer<dimension::three>;
} // namespace lynx
