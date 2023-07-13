#ifndef LYNX_UTILITY_HPP
#define LYNX_UTILITY_HPP

namespace lynx
{
class non_copyable
{
    non_copyable(const non_copyable &) = delete;
    non_copyable &operator=(const non_copyable &) = delete;

  protected:
    non_copyable() = default;
};

class custom_copyable
{
    custom_copyable(const custom_copyable &) = default;
    custom_copyable &operator=(const custom_copyable &) = default;

  protected:
    custom_copyable() = default;
};
} // namespace lynx

#endif