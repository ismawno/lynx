#ifdef KIT_USE_YAML_CPP

#include "lynx/internal/pch.hpp"
#include "lynx/app/layer.hpp"

namespace lynx
{
template <typename Dim> layer<Dim>::layer(const std::string &name) : kit::identifiable<std::string>(name)
{
}

template class layer<dimension::two>;
template class layer<dimension::three>;
} // namespace lynx

#endif
