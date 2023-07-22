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
    node["Name"] = id();
    node["Enabled"] = enabled();
    return node;
}

bool layer::decode(const YAML::Node &node)
{
    KIT_ASSERT_ERROR(node["Name"].as<std::string>() != id(),
                     "Layer to be deserialized must have the same name as the one contained in the YAML node")

    if (!node.IsMap() || node.size() < 2 || node["Name"].as<std::string>() != id())
        return false;
    enabled(node["Enabled"].as<bool>());
    return true;
}
} // namespace lynx

#endif
