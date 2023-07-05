#ifndef LYNX_NON_COPYABLE_HPP
#define LYNX_NON_COPYABLE_HPP

namespace lynx
{
class non_copyable
{
    non_copyable(const non_copyable &) = delete;
    non_copyable &operator=(const non_copyable &) = delete;

  protected:
    non_copyable() = default;
};
} // namespace lynx

#endif