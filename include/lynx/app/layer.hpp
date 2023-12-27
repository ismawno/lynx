#pragma once

#include "lynx/app/input.hpp"
#include "lynx/internal/dimension.hpp"
#include "kit/interface/identifiable.hpp"
#include "kit/interface/toggleable.hpp"
#include "kit/serialization/yaml/serializer.hpp"

#include <functional>
#include <vulkan/vulkan.hpp>

namespace lynx
{
template <typename Dim> class app;
template <typename Dim>
class layer : public kit::identifiable<std::string>,
              public kit::toggleable,
              public kit::yaml::serializable,
              public kit::yaml::deserializable
{
  public:
    using app_t = app<Dim>;
    using event_t = event<Dim>;

    layer(const std::string &name);
    virtual ~layer() = default;

    template <typename T = app_t> T *parent() const
    {
        static_assert(std::is_base_of_v<app_t, T>, "Type must inherit from app class");
        if constexpr (std::is_same_v<app_t, T>)
            return m_parent;
        else
            return dynamic_cast<T *>(m_parent);
    }

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

    template <typename T> friend class app;
};

using layer2D = layer<dimension::two>;
using layer3D = layer<dimension::three>;
} // namespace lynx
