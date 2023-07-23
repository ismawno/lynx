#ifdef KIT_USE_YAML_CPP

#include "lynx/internal/pch.hpp"
#include "lynx/app/layer.hpp"

namespace lynx
{
layer::layer(const std::string &name) : kit::identifiable<std::string>(name)
{
}

YAML::Node layer::encode() const
{
    YAML::Node node;
    node["Enabled"] = enabled();
    return node;
}

bool layer::decode(const YAML::Node &node)
{
    if (!node.IsMap() || node.size() < 1)
        return false;
    enabled(node["Enabled"].as<bool>());
    return true;
}
} // namespace lynx

#endif
