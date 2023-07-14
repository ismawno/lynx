#ifndef LYNX_CUSTOM_COPYABLE_HPP
#define LYNX_CUSTOM_COPYABLE_HPP

namespace lynx
{
class custom_copyable
{
    custom_copyable(const custom_copyable &) = default;
    custom_copyable &operator=(const custom_copyable &) = default;

  protected:
    custom_copyable() = default;
};
} // namespace lynx

#endif