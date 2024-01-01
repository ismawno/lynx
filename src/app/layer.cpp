#include "lynx/internal/pch.hpp"
#include "lynx/serialization/serialization.hpp"
#include "lynx/app/layer.hpp"

namespace lynx
{
template <typename Dim> layer<Dim>::layer(const std::string &name) : kit::identifiable<std::string>(name)
{
}

#ifdef KIT_USE_YAML_CPP
template <typename Dim> YAML::Node layer<Dim>::encode() const
{
    return kit::yaml::codec<layer<Dim>>::encode(*this);
}
template <typename Dim> bool layer<Dim>::decode(const YAML::Node &node)
{
    return kit::yaml::codec<layer<Dim>>::decode(node, *this);
}
#endif

template class layer<dimension::two>;
template class layer<dimension::three>;
} // namespace lynx
