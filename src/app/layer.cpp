#ifdef KIT_USE_YAML_CPP

#include "lynx/internal/pch.hpp"
#include "lynx/app/layer.hpp"

namespace lynx
{
template <typename Dim> layer<Dim>::layer(const std::string &name) : kit::identifiable<std::string>(name)
{
}

template <typename Dim> YAML::Node layer<Dim>::encode() const
{
    YAML::Node node;
    node["Enabled"] = enabled;
    return node;
}

template <typename Dim> bool layer<Dim>::decode(const YAML::Node &node)
{
    if (!node.IsMap() || node.size() < 1)
        return false;
    enabled = node["Enabled"].as<bool>();
    return true;
}
} // namespace lynx

#endif
